#include "WebSocketClient.h"
#include <iostream>

WebSocketClient::WebSocketClient() : is_connected_(false) {
    // We can set logging behavior here if needed
    client_.clear_access_channels(websocketpp::log::alevel::all);
    client_.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
    client_.init_asio();

    // Set up our callback handlers
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;

    client_.set_open_handler(bind(&WebSocketClient::on_open, this, _1));
    client_.set_fail_handler(bind(&WebSocketClient::on_fail, this, _1));
    client_.set_message_handler(bind(&WebSocketClient::on_message, this, _1, _2));
    client_.set_close_handler(bind(&WebSocketClient::on_close, this, _1));
}

WebSocketClient::~WebSocketClient() {
    if (is_connected_) {
        close();
    }
}

void WebSocketClient::connect(const std::string& uri) {
    websocketpp::lib::error_code ec;
    Client::connection_ptr con = client_.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Could not create connection: " << ec.message() << std::endl;
        return;
    }

    client_.connect(con);
    
    // Start the ASIO io_service run loop in a separate thread
    client_thread_ = std::thread([this]() {
        try {
            this->client_.run();
        } catch (const std::exception& e) {
            std::cerr << "WebSocket client thread exception: " << e.what() << std::endl;
        }
    });
}

void WebSocketClient::close() {
    websocketpp::lib::error_code ec;
    client_.close(connection_hdl_, websocketpp::close::status::going_away, "", ec);
    if (ec) {
        std::cerr << "Error closing connection: " << ec.message() << std::endl;
    }
    
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
}

void WebSocketClient::subscribe(const std::string& symbol) {
    if (!is_connected_) {
        std::cerr << "Not connected. Cannot subscribe." << std::endl;
        return;
    }

    // Example subscription message (adjust for your specific API)
    json sub_msg;
    sub_msg["type"] = "subscribe";
    sub_msg["symbol"] = symbol;

    websocketpp::lib::error_code ec;
    client_.send(connection_hdl_, sub_msg.dump(), websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "Error sending subscription: " << ec.message() << std::endl;
    }
}

bool WebSocketClient::get_message(json& msg) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    // Wait until the queue is not empty or the connection is lost
    cv_.wait(lock, [this] { return !message_queue_.empty() || !is_connected_; });

    if (!is_connected_ && message_queue_.empty()) {
        return false; // No more messages and disconnected
    }

    msg = message_queue_.front();
    message_queue_.pop();
    return true;
}

void WebSocketClient::on_open(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket connection opened." << std::endl;
    connection_hdl_ = hdl;
    is_connected_ = true;
}

void WebSocketClient::on_fail(websocketpp::connection_hdl hdl) {
    std::cerr << "WebSocket connection failed." << std::endl;
    is_connected_ = false;
    cv_.notify_all(); // Wake up any waiting threads
}

void WebSocketClient::on_message(websocketpp::connection_hdl hdl, MessagePtr msg) {
    try {
        json parsed_msg = json::parse(msg->get_payload());
        
        { // Lock scope
            std::lock_guard<std::mutex> lock(queue_mutex_);
            message_queue_.push(parsed_msg);
        }
        cv_.notify_one(); // Notify one waiting thread that a message is ready
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

void WebSocketClient::on_close(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket connection closed." << std::endl;
    is_connected_ = false;
    cv_.notify_all(); // Wake up any waiting threads
}
