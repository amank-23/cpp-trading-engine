#pragma once

#include "order_book/Trade.h"
#include "order_book/Order.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

struct Position {
    std::string symbol;
    long long net_position = 0;
    double avg_entry_price = 0.0;
    double realized_pnl = 0.0;
    
    // For simplicity in this step, we'll focus on tracking the position.
    // P&L calculation would be the next enhancement.
};

class RiskEngine {
public:
    RiskEngine(double max_pos_limit);

    // Update position based on an executed trade
    void update_on_trade(const Trade& trade, OrderSide our_order_side, const std::string& symbol);

    // Pre-trade check to see if an order would breach limits
    bool check_pre_trade_risk(const Order& order);

    // Get the current position for a symbol
    std::optional<Position> get_position(const std::string& symbol);

private:
    std::unordered_map<std::string, Position> portfolio_;
    double max_position_limit_;
    std::mutex risk_mutex_;
};
