#include <gtest/gtest.h>
#include "order_book/OrderBook.h"
#include <memory>
#include <vector>

// Test fixture for creating a new OrderBook for each test
class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        book = std::make_unique<OrderBook>();
    }

    std::unique_ptr<OrderBook> book;
public:
    static uint64_t order_id_counter;
};

uint64_t OrderBookTest::order_id_counter = 1;

// Helper to create a new order with a unique ID
std::shared_ptr<Order> create_order(OrderType type, OrderSide side, double price, uint64_t quantity) {
    return std::make_shared<Order>(OrderBookTest::order_id_counter++, "TEST-SYMBOL", type, side, price, quantity);
}

// Test 1: Add a single limit order and check the book depth
TEST_F(OrderBookTest, AddSingleLimitOrder) {
    auto order = create_order(OrderType::LIMIT, OrderSide::BUY, 100.0, 10);
    book->add_order(order);

    auto depth = book->get_depth(OrderSide::BUY);
    ASSERT_EQ(depth.size(), 1);
    EXPECT_EQ(depth[0].first, 100.0);
    EXPECT_EQ(depth[0].second, 10);
}

// Test 2: Simple match between one bid and one ask
TEST_F(OrderBookTest, SimpleFullMatch) {
    std::vector<Trade> trades;
    book->on_trade([&](const Trade& trade) {
        trades.push_back(trade);
    });

    auto buy_order = create_order(OrderType::LIMIT, OrderSide::BUY, 101.0, 50);
    auto sell_order = create_order(OrderType::LIMIT, OrderSide::SELL, 101.0, 50);

    book->add_order(buy_order);
    book->add_order(sell_order);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_EQ(trades[0].price, 101.0);

    // Book should be empty after the match
    EXPECT_TRUE(book->get_depth(OrderSide::BUY).empty());
    EXPECT_TRUE(book->get_depth(OrderSide::SELL).empty());
}

// Test 3: Partial match where the incoming order is smaller
TEST_F(OrderBookTest, PartialMatch) {
    auto resting_sell = create_order(OrderType::LIMIT, OrderSide::SELL, 99.5, 100);
    book->add_order(resting_sell);

    auto aggressive_buy = create_order(OrderType::LIMIT, OrderSide::BUY, 99.5, 20);
    book->add_order(aggressive_buy);
    
    // After the match, the sell side should have 80 remaining
    auto sell_depth = book->get_depth(OrderSide::SELL);
    ASSERT_EQ(sell_depth.size(), 1);
    EXPECT_EQ(sell_depth[0].second, 80);

    // Buy side should be empty
    EXPECT_TRUE(book->get_depth(OrderSide::BUY).empty());
}

// Test 4: Price-Time priority matching
TEST_F(OrderBookTest, PriceTimePriority) {
    // First order: Buy at 100.0
    auto buy1 = create_order(OrderType::LIMIT, OrderSide::BUY, 100.0, 10);
    book->add_order(buy1);

    // Second order: Better price, should match first
    auto buy2 = create_order(OrderType::LIMIT, OrderSide::BUY, 101.0, 20);
    book->add_order(buy2);

    std::vector<Trade> trades;
    book->on_trade([&](const Trade& trade) {
        trades.push_back(trade);
    });
    
    auto sell_order = create_order(OrderType::LIMIT, OrderSide::SELL, 100.5, 50);
    book->add_order(sell_order);
    
    // It should match against the best bid (101.0) first
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].resting_order_id, buy2->id); // Matched against the 101.0 order
    EXPECT_EQ(trades[0].quantity, 20);

    // The 101.0 order should be gone, 100.0 should remain
    auto buy_depth = book->get_depth(OrderSide::BUY);
    ASSERT_EQ(buy_depth.size(), 1);
    EXPECT_EQ(buy_depth[0].first, 100.0);
}

// Test 5: Order Cancellation
TEST_F(OrderBookTest, CancelOrder) {
    auto order_to_cancel = create_order(OrderType::LIMIT, OrderSide::BUY, 100.0, 10);
    book->add_order(order_to_cancel);

    auto depth_before = book->get_depth(OrderSide::BUY);
    ASSERT_EQ(depth_before.size(), 1);

    book->cancel_order(order_to_cancel->id);
    
    // Create a matching order to force the book to purge the cancelled one
    auto trigger_match = create_order(OrderType::LIMIT, OrderSide::SELL, 100.0, 1);
    book->add_order(trigger_match);

    auto depth_after = book->get_depth(OrderSide::BUY);
    EXPECT_TRUE(depth_after.empty());
}
