#include "OrderBook.h"
#include <iostream>
#include <algorithm>

OrderBook::OrderBook() : next_trade_id_(1) {}

void OrderBook::on_trade(TradeCallback callback) {
    trade_callback_ = callback;
}

void OrderBook::add_order(std::shared_ptr<Order> order) {
    std::lock_guard<std::mutex> lock(book_mutex_);
    
    // Store order for quick lookup
    orders_map_[order->id] = order;

    if (order->type == OrderType::LIMIT) {
        add_limit_order(std::move(order));
    } else if (order->type == OrderType::MARKET) {
        add_market_order(std::move(order));
    }

    match_orders();
}

void OrderBook::cancel_order(uint64_t order_id) {
    std::lock_guard<std::mutex> lock(book_mutex_);
    auto it = orders_map_.find(order_id);
    if (it != orders_map_.end()) {
        // "Lazy cancellation": just mark the remaining quantity as 0.
        // The order will be purged when it's next encountered at the top of a price level.
        it->second->remaining_quantity = 0;
        orders_map_.erase(it);
    }
}

void OrderBook::add_limit_order(std::shared_ptr<Order> order) {
    if (order->side == OrderSide::BUY) {
        bids_[order->price].push(order);
    } else {
        asks_[order->price].push(order);
    }
}

void OrderBook::add_market_order(std::shared_ptr<Order> order) {
    // Market orders don't go on the book, they match immediately
    // We handle the matching in the match_orders function
    // For simplicity, we can think of them as limit orders with aggressive prices
    // but the matching logic below will handle them directly.
}


void OrderBook::match_orders() {
    while (!bids_.empty() && !asks_.empty() && bids_.begin()->first >= asks_.begin()->first) {
        auto& best_bid_level = bids_.begin()->second;
        auto& best_ask_level = asks_.begin()->second;
        
        while (!best_bid_level.empty() && !best_ask_level.empty()) {
            auto& bid_order = best_bid_level.front();
            auto& ask_order = best_ask_level.front();

            // Purge cancelled orders
            if (bid_order->remaining_quantity == 0) {
                best_bid_level.pop();
                continue;
            }
            if (ask_order->remaining_quantity == 0) {
                best_ask_level.pop();
                continue;
            }

            uint64_t trade_quantity = std::min(bid_order->remaining_quantity, ask_order->remaining_quantity);
            double trade_price = (bid_order->timestamp < ask_order->timestamp) ? bid_order->price : ask_order->price;

            if (trade_callback_) {
                trade_callback_(Trade(next_trade_id_++, bid_order->id, ask_order->id, trade_price, trade_quantity));
            }

            bid_order->remaining_quantity -= trade_quantity;
            ask_order->remaining_quantity -= trade_quantity;

            if (bid_order->remaining_quantity == 0) {
                orders_map_.erase(bid_order->id);
                best_bid_level.pop();
            }
            if (ask_order->remaining_quantity == 0) {
                orders_map_.erase(ask_order->id);
                best_ask_level.pop();
            }
        }

        if (best_bid_level.empty()) {
            bids_.erase(bids_.begin());
        }
        if (best_ask_level.empty()) {
            asks_.erase(asks_.begin());
        }
    }
}


std::vector<std::pair<double, uint64_t>> OrderBook::get_depth(OrderSide side) {
    std::lock_guard<std::mutex> lock(book_mutex_);
    std::vector<std::pair<double, uint64_t>> depth;
    
    if (side == OrderSide::BUY) {
        for (const auto& [price, level] : bids_) {
            uint64_t total_quantity = 0;
            // A copy of the queue to inspect it without modifying the original
            std::queue<std::shared_ptr<Order>> temp_queue = level;
            while(!temp_queue.empty()) {
                total_quantity += temp_queue.front()->remaining_quantity;
                temp_queue.pop();
            }
            if (total_quantity > 0) {
                depth.emplace_back(price, total_quantity);
            }
        }
    } else { // SELL
        for (const auto& [price, level] : asks_) {
            uint64_t total_quantity = 0;
            std::queue<std::shared_ptr<Order>> temp_queue = level;
            while(!temp_queue.empty()) {
                total_quantity += temp_queue.front()->remaining_quantity;
                temp_queue.pop();
            }
             if (total_quantity > 0) {
                depth.emplace_back(price, total_quantity);
            }
        }
    }
    return depth;
}
