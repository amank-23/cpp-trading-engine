#pragma once

#include <cstdint>
#include <chrono>
#include <string>

enum class OrderType {
    LIMIT,
    MARKET
};

enum class OrderSide {
    BUY,
    SELL
};

struct Order {
    uint64_t id;
    std::string symbol;
    OrderType type;
    OrderSide side;
    double price;
    uint64_t quantity;
    uint64_t remaining_quantity;
    std::chrono::system_clock::time_point timestamp;

    Order(uint64_t p_id, const std::string& p_symbol, OrderType p_type, OrderSide p_side, double p_price, uint64_t p_quantity)
        : id(p_id),
          symbol(p_symbol),
          type(p_type),
          side(p_side),
          price(p_price),
          quantity(p_quantity),
          remaining_quantity(p_quantity),
          timestamp(std::chrono::system_clock::now()) {}
};
