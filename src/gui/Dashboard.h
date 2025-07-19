#pragma once

#include "order_book/OrderBook.h"
#include "risk/RiskEngine.h"
#include <vector>
#include <mutex>

// Forward declare GLFWwindow
struct GLFWwindow;

class Dashboard {
public:
    Dashboard(OrderBook& book, RiskEngine& risk);
    ~Dashboard();

    // The main entry point to start the GUI
    void run();

    // Public method for other threads to add trades to our history
    void add_trade_to_history(const Trade& trade);

private:
    void setup();
    void render_frame();
    void cleanup();

    void render_order_book_panel();
    void render_pnl_position_panel();
    void render_trade_history_panel();

    GLFWwindow* window_;
    OrderBook& order_book_;
    RiskEngine& risk_engine_;

    std::vector<Trade> trade_history_;
    std::mutex history_mutex_;
};
