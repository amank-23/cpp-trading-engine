#include "Dashboard.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include <numeric>

Dashboard::Dashboard(OrderBook& book, RiskEngine& risk)
    : window_(nullptr), order_book_(book), risk_engine_(risk) {}

Dashboard::~Dashboard() {
    cleanup();
}

void Dashboard::run() {
    setup();
    std::cout << "[DASHBOARD] GUI started. Close the window to shutdown the trading system." << std::endl;

    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        render_frame();
        glfwSwapBuffers(window_);
    }
    
    std::cout << "[DASHBOARD] GUI window closed." << std::endl;
}

void Dashboard::add_trade_to_history(const Trade& trade) {
    std::lock_guard<std::mutex> lock(history_mutex_);
    trade_history_.push_back(trade);
    if (trade_history_.size() > 50) { // Keep history to a reasonable size
        trade_history_.erase(trade_history_.begin());
    }
}

void Dashboard::setup() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(1600, 900, "Real-Time Trading System Dashboard", NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Increase font size for better visibility in screenshots/videos
    io.FontGlobalScale = 1.8f;  // Make text 80% larger
    
    // Note: Docking might not be available in all ImGui versions
    
    // Setup style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
    
    // Scale up UI elements to match larger font
    style.ScaleAllSizes(1.5f);  // Scale buttons, padding, etc.

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Dashboard::render_frame() {
    // Clear background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render panels directly without docking
    render_order_book_panel();
    render_pnl_position_panel();
    render_trade_history_panel();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Dashboard::cleanup() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

void Dashboard::render_order_book_panel() {
    ImGui::Begin("📊 Order Book");
    
    auto bids = order_book_.get_depth(OrderSide::BUY);
    auto asks = order_book_.get_depth(OrderSide::SELL);

    if (ImGui::BeginTable("OrderBookTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Bids", ImGuiTableColumnFlags_WidthFixed, 300.0f);
        ImGui::TableSetupColumn("Asks", ImGuiTableColumnFlags_WidthFixed, 300.0f);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        
        // Bids section
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "BUY ORDERS");
        if (ImGui::BeginTable("BidsTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();
            
            for (const auto& [price, qty] : bids) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "%.2f", price);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", qty);
            }
            ImGui::EndTable();
        }

        ImGui::TableSetColumnIndex(1);
        
        // Asks section
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "SELL ORDERS");
        if (ImGui::BeginTable("AsksTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();
            
            for (const auto& [price, qty] : asks) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%.2f", price);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", qty);
            }
            ImGui::EndTable();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void Dashboard::render_pnl_position_panel() {
    ImGui::Begin("💼 Portfolio & Risk");
    
    auto pos = risk_engine_.get_position("BTC-USD");
    if (pos) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Symbol: %s", pos->symbol.c_str());
        
        // Color position based on long/short
        ImVec4 pos_color = pos->net_position >= 0 ? 
            ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :  // Green for long
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f);   // Red for short
            
        ImGui::TextColored(pos_color, "Net Position: %lld", pos->net_position);
        ImGui::Text("Avg Entry: %.2f", pos->avg_entry_price);
        ImGui::Text("Realized P&L: %.2f", pos->realized_pnl);
        
        // Position status
        ImGui::Separator();
        if (pos->net_position > 0) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "📈 LONG POSITION");
        } else if (pos->net_position < 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "📉 SHORT POSITION");
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "🔄 FLAT");
        }
    } else {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "No position for BTC-USD");
    }
    
    ImGui::Separator();
    ImGui::Text("🛡️ Risk Management:");
    ImGui::Text("Max Position Limit: %.0f", 80.0); // We know this from our setup
    
    ImGui::End();
}

void Dashboard::render_trade_history_panel() {
    ImGui::Begin("📋 Trade History");
    
    ImGui::Text("Recent Executions:");
    ImGui::Separator();
    
    if (ImGui::BeginTable("HistoryTable", 4, 
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        
        ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Resting ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Aggr. ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();
        
        std::lock_guard<std::mutex> lock(history_mutex_);
        for (auto it = trade_history_.rbegin(); it != trade_history_.rend(); ++it) {
            const auto& trade = *it;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f", trade.price);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%lu", trade.quantity);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%lu", trade.resting_order_id);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%lu", trade.aggressive_order_id);
        }
        ImGui::EndTable();
    }
    
    if (trade_history_.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No trades executed yet...");
    }
    
    ImGui::End();
}
