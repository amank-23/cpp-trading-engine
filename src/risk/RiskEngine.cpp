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
    double trade_price = trade.price;
    
    // Store old position for P&L calculation
    long long old_position = pos.net_position;
    double old_avg_price = pos.avg_entry_price;

    // Update net position
    if (our_order_side == OrderSide::BUY) {
        pos.net_position += trade_size;
    } else { // SELL
        pos.net_position -= trade_size;
    }

    // Calculate average entry price and realized P&L
    if (our_order_side == OrderSide::BUY) {
        // Buying: update average entry price
        if (old_position >= 0) {
            // Adding to long position or opening long position
            double total_cost = (old_position * old_avg_price) + (trade_size * trade_price);
            pos.avg_entry_price = (pos.net_position > 0) ? total_cost / pos.net_position : trade_price;
        } else if (pos.net_position >= 0) {
            // Covered short position and possibly went long
            pos.realized_pnl += (old_avg_price - trade_price) * std::min(trade_size, -old_position);
            if (pos.net_position > 0) {
                pos.avg_entry_price = trade_price; // New long position
            }
        }
    } else { // SELL
        // Selling: realize P&L if closing/reducing long, or update avg for short
        if (old_position > 0) {
            // Reducing/closing long position
            pos.realized_pnl += (trade_price - old_avg_price) * std::min(trade_size, old_position);
            if (pos.net_position <= 0 && pos.net_position < 0) {
                pos.avg_entry_price = trade_price; // New short position
            }
        } else {
            // Adding to short position or opening short position
            if (old_position <= 0) {
                double total_cost = (-old_position * old_avg_price) + (trade_size * trade_price);
                pos.avg_entry_price = (pos.net_position < 0) ? total_cost / (-pos.net_position) : trade_price;
            }
        }
    }

    std::cout << "[RISK ENGINE] Updated position for " << symbol 
              << ". Position: " << pos.net_position 
              << ", Avg Entry: $" << pos.avg_entry_price 
              << ", Realized P&L: $" << pos.realized_pnl << std::endl;
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
