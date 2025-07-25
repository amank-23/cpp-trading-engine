<div align="center">

# Real-Time Trading System

**A complete, multi-threaded real-time trading system built in C++**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)](https://cmake.org/)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/amank-23/cpp-trading-engine)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://github.com/amank-23/cpp-trading-engine)

**[View Demo](#demo) · [Documentation](#documentation) · [Report Bug](https://github.com/amank-23/cpp-trading-engine/issues) · [Request Feature](https://github.com/amank-23/cpp-trading-engine/issues)**

</div>

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Demo](#demo)
- [Architecture](#architecture)
- [Installation](#installation)
- [Usage](#usage)
- [Testing](#testing)
- [License](#license)

---

## Overview

A high-performance, enterprise-grade trading system implementing core financial market infrastructure components. This system demonstrates advanced C++ programming techniques, real-time data processing, and financial software architecture patterns used in production trading environments.

### Key Highlights
- **Multi-threaded Architecture**: Concurrent order processing and GUI updates
- **Real-time GUI**: Live ImGui dashboard with order book visualization
- **Risk Management**: Pre-trade validation and position tracking
- **WebSocket Integration**: Real-time market data handling
- **Cross-platform**: Linux, Windows, and macOS support

---

## Features

<table>
<tr>
<td width="50%">

### Core Trading Engine
- **Order Book**: Price-time priority matching
- **Order Types**: Limit orders with market order support
- **Trade Execution**: Real-time matching engine
- **Order Management**: Add, modify, cancel operations

### Market Data
- **WebSocket Client**: Real-time data ingestion
- **JSON Processing**: High-performance parsing
- **Message Queue**: Thread-safe producer-consumer
- **Data Simulation**: Built-in market data simulator

</td>
<td width="50%">

### Risk Management
- **Position Tracking**: Real-time portfolio monitoring
- **Pre-trade Checks**: Risk validation before execution
- **Exposure Limits**: Configurable position limits
- **P&L Calculation**: Real-time profit/loss tracking

### Live Dashboard
- **Order Book Display**: Real-time bid/ask visualization
- **Portfolio Panel**: Position and P&L monitoring  
- **Trade History**: Execution log with full details
- **Risk Metrics**: Live risk monitoring display

</td>
</tr>
</table>

---

## Demo

### Live Trading System in Action

**[View Demo Video](https://github.com/amank-23/cpp-trading-engine/raw/main/assets/videos/Demo.mp4)**

*30-second demo showing real-time order matching, GUI updates, and risk management*

### Screenshots

<div align="center">

#### Real-Time Dashboard
![Dashboard Overview](assets/screenshots/dashboard-overview.png)
*Complete trading dashboard with order book, portfolio, and trade history*

</div>

<table>
<tr>
<td width="33%">
  <div align="center">
    <img src="assets/screenshots/order-book.png" alt="Order Book" width="100%"/>
    <br><strong>Order Book</strong><br>
    <em>Live bid/ask display with price-time priority</em>
  </div>
</td>
<td width="33%">
  <div align="center">
    <img src="assets/screenshots/portfolio.png" alt="Portfolio" width="100%"/>
    <br><strong>Portfolio</strong><br>
    <em>Real-time position and P&L tracking</em>
  </div>
</td>
<td width="33%">
  <div align="center">
    <img src="assets/screenshots/trades.png" alt="Trade History" width="100%"/>
    <br><strong>Trade History</strong><br>
    <em>Execution log with full trade details</em>
  </div>
</td>
</tr>
</table>

---

## Architecture

<div align="center">

```mermaid
graph TB
    A[WebSocket Market Data] --> B[Order Processing Thread]
    B --> C[Risk Engine]
    C --> D[Order Book Engine]
    D --> E[Trade Execution]
    E --> F[Position Update]
    F --> G[GUI Dashboard]
    
    D --> H[Order Book Display]
    F --> I[Portfolio Panel]  
    E --> J[Trade History]
    
    H --> G
    I --> G
    J --> G
    
    style A fill:#e1f5fe
    style D fill:#f3e5f5
    style G fill:#e8f5e8
```

</div>

### Component Overview

| Component | Description | Key Features |
|-----------|-------------|--------------|
| **Order Book** | Core matching engine | Price-time priority, O(log n) operations |
| **WebSocket Client** | Market data handler | Async I/O, message queuing |
| **Risk Engine** | Risk management | Pre-trade validation, position tracking |
| **Dashboard** | GUI visualization | Real-time updates, ImGui framework |

---

## Installation

### Prerequisites

<table>
<tr>
<td width="50%">

**System Requirements**
- **OS**: Linux (Ubuntu 20.04+), Windows 10+, macOS 10.15+
- **Compiler**: GCC 9+, Clang 10+, MSVC 2019+
- **CMake**: 3.16 or higher
- **Memory**: 4GB RAM minimum
- **Storage**: 1GB free space

</td>
<td width="50%">

**Dependencies**
- **Boost**: System utilities and threading
- **websocketpp**: WebSocket client library
- **nlohmann/json**: JSON parsing
- **Google Test**: Unit testing framework
- **ImGui**: GUI framework
- **GLFW + OpenGL**: Graphics rendering

</td>
</tr>
</table>

### Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/amank-23/cpp-trading-engine.git
cd cpp-trading-engine

# 2. Initialize submodules
git submodule update --init --recursive

# 3. Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install -y build-essential cmake pkg-config
sudo apt install -y libboost-system-dev nlohmann-json3-dev libwebsocketpp-dev
sudo apt install -y libgtest-dev libglfw3-dev libgl1-mesa-dev
sudo apt install -y libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# 4. Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# 5. Run the system
./TradingSystem
```

### Alternative Installation Methods

<details>
<summary><b>🐳 Docker Installation</b></summary>

```bash
# Build Docker image
docker build -t trading-system .

# Run with GUI support (Linux)
docker run -it --rm \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  trading-system
```
</details>

<details>
<summary><b>📦 Package Manager Installation</b></summary>

```bash
# Using vcpkg
vcpkg install boost nlohmann-json websocketpp gtest glfw3

# Using Conan
conan install . --install-folder=build --build=missing
```
</details>

---

## Usage

### Running Different Components

```bash
# Full system with GUI
./TradingSystem

# Backend-only test (no GUI required)
./BackendTest

# Unit tests
./RunTests

# Interactive launcher
../run.sh
```

### Configuration

<details>
<summary><b>⚙️ System Configuration</b></summary>

```cpp
// Risk management settings
RiskEngine risk_engine(80.0);  // Max position size

// WebSocket connection
ws_client->connect("ws://your-market-data-feed.com");

// GUI settings
Dashboard dashboard(*order_book, *risk_engine);
```
</details>

### API Usage Example

```cpp
#include "order_book/OrderBook.h"
#include "risk/RiskEngine.h"

// Create components
auto order_book = std::make_shared<OrderBook>();
auto risk_engine = std::make_shared<RiskEngine>(100.0);

// Set up trade callback
order_book->on_trade([](const Trade& trade) {
    std::cout << "Trade executed: " << trade.price 
              << " x " << trade.quantity << std::endl;
});

// Create and add order
auto order = std::make_shared<Order>(
    1, "BTC-USD", OrderType::LIMIT, 
    OrderSide::BUY, 50000.0, 1
);

if (risk_engine->check_pre_trade_risk(*order)) {
    order_book->add_order(order);
}
```

---

## Testing

### Running Tests

```bash
# Run unit tests
make test

# Or run directly
./RunTests
```

The project includes unit tests for the OrderBook component using Google Test framework.

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2025 amank-23

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```
