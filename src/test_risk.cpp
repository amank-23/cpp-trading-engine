#include "order_book/OrderBook.h"
#include "market_data/WebSocketClient.h"
#include "risk/RiskEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

// A counter for generating unique order IDs
std::atomic<uint64_t> order_id_counter = 1;

void risk_limit_test() {
    std::cout << "=== Risk Limit Test ===" << std::endl;
    
    RiskEngine risk_engine(50.0); // Very low limit for testing
    
    // Test 1: Normal order should pass
    auto order1 = std::make_shared<Order>(1, "TEST", OrderType::LIMIT, OrderSide::BUY, 100.0, 30);
    bool result1 = risk_engine.check_pre_trade_risk(*order1);
    std::cout << "Order 1 (30 shares): " << (result1 ? "APPROVED" : "REJECTED") << std::endl;
    
    // Simulate that order1 was filled
    Trade trade1{1, 1, 2, 100.0, 30};
    risk_engine.update_on_trade(trade1, OrderSide::BUY, "TEST");
    
    // Test 2: This should still pass (30 + 15 = 45 <= 50)
    auto order2 = std::make_shared<Order>(2, "TEST", OrderType::LIMIT, OrderSide::BUY, 101.0, 15);
    bool result2 = risk_engine.check_pre_trade_risk(*order2);
    std::cout << "Order 2 (15 shares): " << (result2 ? "APPROVED" : "REJECTED") << std::endl;
    
    // Test 3: This should be REJECTED (30 + 25 = 55 > 50)
    auto order3 = std::make_shared<Order>(3, "TEST", OrderType::LIMIT, OrderSide::BUY, 102.0, 25);
    bool result3 = risk_engine.check_pre_trade_risk(*order3);
    std::cout << "Order 3 (25 shares): " << (result3 ? "APPROVED" : "REJECTED") << std::endl;
    
    // Test 4: Sell order to reduce position should be allowed
    auto order4 = std::make_shared<Order>(4, "TEST", OrderType::LIMIT, OrderSide::SELL, 99.0, 40);
    bool result4 = risk_engine.check_pre_trade_risk(*order4);
    std::cout << "Order 4 (SELL 40 shares): " << (result4 ? "APPROVED" : "REJECTED") << std::endl;
    
    std::cout << "Risk limit test completed!" << std::endl;
}

int main() {
    risk_limit_test();
    return 0;
}
