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

/**
 * @brief Simple backend test - processes a few orders to demonstrate the system works
 */
void test_backend_functionality() {
    std::cout << "=== TESTING BACKEND TRADING SYSTEM ===" << std::endl;
    
    // 1. Initialize components
    auto order_book = std::make_shared<OrderBook>();
    auto risk_engine = std::make_shared<RiskEngine>(80.0);
    
    int trade_count = 0;
    
    // 2. Set up trade callback 
    order_book->on_trade([&](const Trade& trade) {
        trade_count++;
        std::cout << "\n🎯 TRADE #" << trade_count << " EXECUTED:" << std::endl;
        std::cout << "   💰 Price: $" << trade.price << std::endl;
        std::cout << "   📊 Quantity: " << trade.quantity << std::endl;
        std::cout << "   🆔 Resting: " << trade.resting_order_id << " | Aggressive: " << trade.aggressive_order_id << std::endl;
        
        // Update risk engine
        risk_engine->update_on_trade(trade, OrderSide::BUY, "BTC-USD");
        std::cout << "   ✅ Risk engine updated." << std::endl;
        
        // Show position
        auto pos = risk_engine->get_position("BTC-USD");
        if (pos) {
            std::cout << "   📈 New Position: " << pos->net_position << " @ avg $" << pos->avg_entry_price << std::endl;
        }
        std::cout << "═══════════════════════════════════════\n" << std::endl;
    });

    std::cout << "\n1️⃣  Creating test orders..." << std::endl;
    
    // Create some test orders to demonstrate matching
    auto buy1 = std::make_shared<Order>(order_id_counter++, "BTC-USD", OrderType::LIMIT, OrderSide::BUY, 100.0, 10);
    auto buy2 = std::make_shared<Order>(order_id_counter++, "BTC-USD", OrderType::LIMIT, OrderSide::BUY, 99.0, 15);
    auto sell1 = std::make_shared<Order>(order_id_counter++, "BTC-USD", OrderType::LIMIT, OrderSide::SELL, 101.0, 8);
    
    std::cout << "\n2️⃣  Adding orders to book (with risk checks)..." << std::endl;
    
    // Add orders with risk checks
    if (risk_engine->check_pre_trade_risk(*buy1)) {
        order_book->add_order(buy1);
        std::cout << "   ✅ Buy order at $100.0 for 10 BTC - APPROVED" << std::endl;
    }
    
    if (risk_engine->check_pre_trade_risk(*buy2)) {
        order_book->add_order(buy2);
        std::cout << "   ✅ Buy order at $99.0 for 15 BTC - APPROVED" << std::endl;
    }
    
    if (risk_engine->check_pre_trade_risk(*sell1)) {
        order_book->add_order(sell1);
        std::cout << "   ✅ Sell order at $101.0 for 8 BTC - APPROVED" << std::endl;
    }
    
    std::cout << "\n3️⃣  Displaying order book depth..." << std::endl;
    auto bids = order_book->get_depth(OrderSide::BUY);
    auto asks = order_book->get_depth(OrderSide::SELL);
    
    std::cout << "   📗 BIDS (Buy Orders):" << std::endl;
    for (const auto& [price, qty] : bids) {
        std::cout << "      $" << price << " @ " << qty << " BTC" << std::endl;
    }
    
    std::cout << "   📕 ASKS (Sell Orders):" << std::endl;
    for (const auto& [price, qty] : asks) {
        std::cout << "      $" << price << " @ " << qty << " BTC" << std::endl;
    }
    
    std::cout << "\n4️⃣  Adding aggressive order that will trigger trades..." << std::endl;
    auto aggressive_sell = std::make_shared<Order>(order_id_counter++, "BTC-USD", OrderType::LIMIT, OrderSide::SELL, 99.0, 20);
    
    if (risk_engine->check_pre_trade_risk(*aggressive_sell)) {
        order_book->add_order(aggressive_sell);
        std::cout << "   🔥 Aggressive sell order added - this should match!" << std::endl;
    }
    
    // Give a moment for any async processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "\n5️⃣  Final portfolio state..." << std::endl;
    auto final_pos = risk_engine->get_position("BTC-USD");
    if (final_pos) {
        std::cout << "   🎯 Final Position: " << final_pos->net_position << " BTC" << std::endl;
        std::cout << "   💵 Avg Entry Price: $" << final_pos->avg_entry_price << std::endl;
        std::cout << "   💰 Realized P&L: $" << final_pos->realized_pnl << std::endl;
    } else {
        std::cout << "   📊 No position in BTC-USD" << std::endl;
    }
    
    std::cout << "\n✨ Backend Test Complete! Total Trades: " << trade_count << " ✨" << std::endl;
    std::cout << "\n📝 The GUI Dashboard would show all this data in real-time!" << std::endl;
    std::cout << "🔗 Run './TradingSystem' in an environment with display server to see the GUI." << std::endl;
}

int main() {
    test_backend_functionality();
    return 0;
}
