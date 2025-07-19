#include "RiskEngine.h"
#include <iostream>
#include <cmath>

RiskEngine::RiskEngine(double max_pos_limit) : max_position_limit_(max_pos_limit) {}

void RiskEngine::update_on_trade(const Trade& trade, OrderSide our_order_side, const std::string& symbol) {
    std::lock_guard<std::mutex> lock(risk_mutex_);

    // Find or create a position for the symbol
    if (portfolio_.find(symbol) == portfolio_.end()) {
        portfolio_[symbol] = Position{symbol};
    }
    
    Position& pos = portfolio_[symbol];
    long long trade_size = static_cast<long long>(trade.quantity);

    // Update net position
    if (our_order_side == OrderSide::BUY) {
        pos.net_position += trade_size;
    } else { // SELL
        pos.net_position -= trade_size;
    }

    // This is a simplified P&L and average price calculation. A real system would be more complex.
    // For now, we focus on tracking the position size correctly.
    std::cout << "[RISK ENGINE] Updated position for " << symbol 
              << ". New Net Position: " << pos.net_position << std::endl;
}

bool RiskEngine::check_pre_trade_risk(const Order& order) {
    std::lock_guard<std::mutex> lock(risk_mutex_);
    
    long long current_pos = 0;
    if (portfolio_.count(order.symbol)) {
        current_pos = portfolio_.at(order.symbol).net_position;
    }

    long long potential_pos_change = (order.side == OrderSide::BUY) ? order.quantity : -static_cast<long long>(order.quantity);
    long long potential_net_pos = current_pos + potential_pos_change;

    if (std::abs(potential_net_pos) > max_position_limit_) {
        std::cerr << "[RISK ENGINE] PRE-TRADE RISK CHECK FAILED: Order would exceed max position limit." << std::endl;
        std::cerr << "   Current Position: " << current_pos << ", Potential Position: " << potential_net_pos 
                  << ", Max Limit: " << max_position_limit_ << std::endl;
        return false;
    }

    std::cout << "[RISK ENGINE] PRE-TRADE RISK CHECK PASSED. Current: " << current_pos 
              << " → Potential: " << potential_net_pos << std::endl;
    return true;
}

std::optional<Position> RiskEngine::get_position(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(risk_mutex_);
    if (portfolio_.count(symbol)) {
        return portfolio_.at(symbol);
    }
    return std::nullopt;
}
