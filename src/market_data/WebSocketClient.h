#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

// Define types for convenience
using Client = websocketpp::client<websocketpp::config::asio>;
using MessagePtr = websocketpp::config::asio::message_type::ptr;
using json = nlohmann::json;

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    // Connect to the WebSocket server
    void connect(const std::string& uri);

    // Close the connection
    void close();

    // Subscribe to a symbol/channel
    void subscribe(const std::string& symbol);
    
    // Thread-safe method to retrieve a message from the queue
    bool get_message(json& msg);

private:
    void on_open(websocketpp::connection_hdl hdl);
    void on_fail(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, MessagePtr msg);
    void on_close(websocketpp::connection_hdl hdl);

    Client client_;
    websocketpp::connection_hdl connection_hdl_;
    std::thread client_thread_;

    std::queue<json> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool is_connected_;
};
