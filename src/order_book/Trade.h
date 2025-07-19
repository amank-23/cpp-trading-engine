#pragma once

#include <cstdint>
#include <chrono>

struct Trade {
    uint64_t trade_id;
    uint64_t resting_order_id;
    uint64_t aggressive_order_id;
    double price;
    uint64_t quantity;
    std::chrono::system_clock::time_point timestamp;

    Trade(uint64_t t_id, uint64_t r_id, uint64_t a_id, double p, uint64_t q)
        : trade_id(t_id),
          resting_order_id(r_id),
          aggressive_order_id(a_id),
          price(p),
          quantity(q),
          timestamp(std::chrono::system_clock::now()) {}
};
