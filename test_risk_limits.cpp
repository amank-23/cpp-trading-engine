#include "src/risk/RiskEngine.h"
#include "src/order_book/Order.h"
#include <iostream>

int main() {
    std::cout << "=== Testing Risk Engine Limits ===" << std::endl;
    
    RiskEngine risk_engine(50.0); // Low limit for testing
    
    // Test 1: Order within limits
    auto order1 = Order(1, "TEST", OrderType::LIMIT, OrderSide::BUY, 100.0, 30);
    std::cout << "Test 1 - Order for 30 units (limit=50): ";
    if (risk_engine.check_pre_trade_risk(order1)) {
        std::cout << "PASSED ✅" << std::endl;
    } else {
        std::cout << "FAILED ❌" << std::endl;
    }
    
    // Test 2: Order that exceeds limits
    auto order2 = Order(2, "TEST", OrderType::LIMIT, OrderSide::BUY, 100.0, 80);
    std::cout << "Test 2 - Order for 80 units (limit=50): ";
    if (risk_engine.check_pre_trade_risk(order2)) {
        std::cout << "FAILED ❌ (should have been rejected)" << std::endl;
    } else {
        std::cout << "PASSED ✅ (correctly rejected)" << std::endl;
    }
    
    std::cout << "=== Risk Engine Tests Complete ===" << std::endl;
    return 0;
}
