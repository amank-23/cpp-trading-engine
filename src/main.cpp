#include "order_book/OrderBook.h"
#include "market_data/WebSocketClient.h"
#include "risk/RiskEngine.h"
#include "gui/Dashboard.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

// A counter for generating unique order IDs
std::atomic<uint64_t> order_id_counter = 1;

/**
 * @brief Processes messages from the WebSocket and updates the OrderBook.
 * Now includes pre-trade risk checking.
 * @param client The WebSocket client to pull messages from.
 * @param book The OrderBook to update.
 * @param risk The RiskEngine for position tracking and limits.
 * @param running An atomic flag to signal when to stop.
 */
void market_data_handler(WebSocketClient& client, OrderBook& book, RiskEngine& risk, std::atomic<bool>& running) {
    std::cout << "[DATA HANDLER] Market data handler started with risk management..." << std::endl;
    json msg;
    int processed_count = 0;
    
    while (running && client.get_message(msg)) {
        processed_count++;
        std::cout << "[DATA HANDLER] Processing message #" << processed_count << std::endl;
        
        try {
            // Check if this is our echoed subscription message
            if (msg.contains("type") && msg["type"] == "subscribe" && msg.contains("symbol")) {
                // Parse the actual order data from the symbol field
                json order_data = json::parse(msg["symbol"].get<std::string>());
                
                if (order_data.contains("type") && order_data["type"] == "limit") {
                    std::string symbol_str = order_data["symbol"];
                    std::string side_str = order_data["side"];
                    double price = order_data["price"];
                    uint64_t quantity = order_data["quantity"];

                    OrderSide side = (side_str == "buy") ? OrderSide::BUY : OrderSide::SELL;
                    
                    auto order = std::make_shared<Order>(
                        order_id_counter++, 
                        symbol_str,
                        OrderType::LIMIT, 
                        side, 
                        price, 
                        quantity
                    );

                    // **PRE-TRADE RISK CHECK**
                    std::cout << "[DATA HANDLER] Checking risk for: " << side_str << " " << quantity << " @ " << price << std::endl;
                    if (risk.check_pre_trade_risk(*order)) {
                        std::cout << "[DATA HANDLER] Order APPROVED and added to book." << std::endl;
                        book.add_order(order);
                    } else {
                        std::cout << "[DATA HANDLER] Order REJECTED by risk engine." << std::endl;
                    }
                } else {
                    std::cout << "[DATA HANDLER] Nested message doesn't contain valid limit order data." << std::endl;
                }
            }
            // Direct limit order format (for future real feeds)
            else if (msg.contains("type") && msg["type"] == "limit") {
                std::string symbol_str = msg["symbol"];
                std::string side_str = msg["side"];
                double price = msg["price"];
                uint64_t quantity = msg["quantity"];

                OrderSide side = (side_str == "buy") ? OrderSide::BUY : OrderSide::SELL;
                
                auto order = std::make_shared<Order>(
                    order_id_counter++, 
                    symbol_str,
                    OrderType::LIMIT, 
                    side, 
                    price, 
                    quantity
                );

                // **PRE-TRADE RISK CHECK**
                if (risk.check_pre_trade_risk(*order)) {
                    std::cout << "[DATA HANDLER] Order APPROVED and added to book." << std::endl;
                    book.add_order(order);
                } else {
                    std::cout << "[DATA HANDLER] Order REJECTED by risk engine." << std::endl;
                }
            } else {
                std::cout << "[DATA HANDLER] Message doesn't contain valid order data." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[DATA HANDLER] Error processing message: " << e.what() << " | Message: " << msg.dump() << std::endl;
        }
    }
    std::cout << "[DATA HANDLER] Market data handler thread finished. Processed " << processed_count << " messages." << std::endl;
}

/**
 * @brief Simulates an exchange sending market data to us.
 * @param client The WebSocket client to send messages through.
 * @param running An atomic flag to signal when to stop.
 */
void simulate_exchange_feed(WebSocketClient& client, std::atomic<bool>& running) {
    std::cout << "[SIMULATOR] Starting exchange feed simulation with risk limits..." << std::endl;
    int i = 0;
    while(running && i < 10) {
        std::cout << "[SIMULATOR] Sending order pair " << (i+1) << "/10" << std::endl;
        
        // Simulate a buy order
        json buy_order;
        buy_order["type"] = "limit";
        buy_order["symbol"] = "BTC-USD";
        buy_order["side"] = "buy";
        buy_order["price"] = 100.0 + i;
        buy_order["quantity"] = 25; // Larger quantities to test risk limits (was 15)
        client.subscribe(buy_order.dump());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Simulate a sell order
        json sell_order;
        sell_order["type"] = "limit";
        sell_order["symbol"] = "BTC-USD";
        sell_order["side"] = "sell";
        sell_order["price"] = 110.0 - i;
        sell_order["quantity"] = 10;
        client.subscribe(sell_order.dump());

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        i++;
    }
    std::cout << "[SIMULATOR] Exchange feed simulation completed." << std::endl;
}


int main() {
    std::cout << "=== Real-Time Trading System with GUI Dashboard ===" << std::endl;
    
    // 1. Initialize components
    auto order_book = std::make_shared<OrderBook>();
    auto ws_client = std::make_shared<WebSocketClient>();
    auto risk_engine = std::make_shared<RiskEngine>(80.0); // Set max position size to 80
    auto dashboard = std::make_shared<Dashboard>(*order_book, *risk_engine);
    
    std::atomic<bool> running(true);

    std::cout << "1. Setting up trade callback with GUI and risk engine..." << std::endl;
    // 2. Set up the trade callback to update both the risk engine and dashboard
    order_book->on_trade([&](const Trade& trade) {
        std::cout << "\n>>> TRADE EXECUTED <<<" << std::endl;
        std::cout << "   Price: " << trade.price << ", Quantity: " << trade.quantity << std::endl;
        std::cout << "   Resting Order ID: " << trade.resting_order_id << ", Aggressive Order ID: " << trade.aggressive_order_id << std::endl;
        
        // Update risk engine and dashboard
        risk_engine->update_on_trade(trade, OrderSide::BUY, "BTC-USD");
        dashboard->add_trade_to_history(trade);
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
    });

    std::cout << "2. Connecting to WebSocket server..." << std::endl;
    // 3. Connect to the WebSocket server
    ws_client->connect("ws://echo.websocket.events");
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for connection

    std::cout << "3. Starting market data handler with risk management..." << std::endl;
    // 4. Start the thread that processes incoming data and updates the order book
    std::thread handler_thread(market_data_handler, std::ref(*ws_client), std::ref(*order_book), std::ref(*risk_engine), std::ref(running));
    
    std::cout << "4. Starting exchange feed simulator..." << std::endl;
    // 5. Start a thread to simulate the exchange sending us data
    std::thread simulator_thread(simulate_exchange_feed, std::ref(*ws_client), std::ref(running));

    std::cout << "5. Launching GUI Dashboard..." << std::endl;
    std::cout << "   Close the GUI window to shutdown the trading system." << std::endl;
    
    // 6. Run the GUI - This will block until the window is closed
    try {
        dashboard->run();
    } catch (const std::exception& e) {
        std::cerr << "Dashboard error: " << e.what() << std::endl;
    }

    // 7. Clean up after GUI closes
    std::cout << "Dashboard closed. Shutting down backend threads..." << std::endl;
    running = false;
    ws_client->close();

    if (handler_thread.joinable()) {
        handler_thread.join();
    }
    if (simulator_thread.joinable()) {
        simulator_thread.join();
    }
    
    std::cout << "All threads stopped. Main application finished." << std::endl;
    return 0;
}
