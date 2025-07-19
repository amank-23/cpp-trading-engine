#pragma once

#include "Order.h"
#include "Trade.h"
#include <map>
#include <queue>
#include <mutex>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class OrderBook {
public:
    using TradeCallback = std::function<void(const Trade&)>;

    OrderBook();

    // Add a new order to the book
    void add_order(std::shared_ptr<Order> order);

    // Cancel an existing order
    void cancel_order(uint64_t order_id);

    // Register a callback for trade events
    void on_trade(TradeCallback callback);
    
    // Get a snapshot of the order book depth
    std::vector<std::pair<double, uint64_t>> get_depth(OrderSide side);

private:
    using PriceLevel = std::queue<std::shared_ptr<Order>>;
    
    // Bids are sorted high-to-low, so we use std::greater
    std::map<double, PriceLevel, std::greater<double>> bids_;
    // Asks are sorted low-to-high, so we use the default std::less
    std::map<double, PriceLevel> asks_;

    // For fast O(1) average time complexity access to orders for cancellation
    std::unordered_map<uint64_t, std::shared_ptr<Order>> orders_map_;

    std::mutex book_mutex_;
    TradeCallback trade_callback_;
    uint64_t next_trade_id_;

    void match_orders();
    void execute_trade(std::shared_ptr<Order>& resting_order, std::shared_ptr<Order>& aggressive_order, PriceLevel& resting_level);
    void add_limit_order(std::shared_ptr<Order> order);
    void add_market_order(std::shared_ptr<Order> order);
};
