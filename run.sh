#!/bin/bash

echo "🚀 Real-Time Trading System Launcher"
echo "===================================="
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found. Please run 'mkdir build && cd build && cmake .. && make' first."
    exit 1
fi

# Change to build directory
cd build

echo "📋 Available commands:"
echo ""
echo "1. 🧪 Run Unit Tests"
echo "   ./RunTests"
echo ""
echo "2. 🔧 Test Backend (No GUI)"
echo "   ./BackendTest"
echo ""
echo "3. 🎮 Launch Full System with GUI Dashboard"
echo "   ./TradingSystem"
echo ""

# Check for GUI support
if [ -n "$DISPLAY" ]; then
    echo "✅ Display server detected: $DISPLAY"
    echo "   GUI should work properly."
else
    echo "⚠️  No display server detected."
    echo "   GUI may not work. Try backend test instead."
fi

echo ""
echo "Choose an option [1-3], or press Enter for backend test:"
read -r choice

case $choice in
    1)
        echo "🧪 Running unit tests..."
        ./RunTests
        ;;
    2|"")
        echo "🔧 Running backend test (no GUI)..."
        ./BackendTest
        ;;
    3)
        echo "🎮 Launching full system with GUI..."
        if [ -n "$DISPLAY" ]; then
            ./TradingSystem
        else
            echo "❌ Cannot launch GUI without display server."
            echo "   Running backend test instead..."
            ./BackendTest
        fi
        ;;
    *)
        echo "❌ Invalid choice. Running backend test..."
        ./BackendTest
        ;;
esac

echo ""
echo "✨ Done!"
