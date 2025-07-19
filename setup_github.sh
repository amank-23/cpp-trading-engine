#!/bin/bash

echo "=== GitHub Setup for cpp-trading-engine ==="
echo ""

# Check if git is initialized
if [ ! -d ".git" ]; then
    echo "Initializing Git repository..."
    git init
else
    echo "Git repository already exists."
fi

# Check git status
echo ""
echo "Current Git status:"
git status --short

echo ""
echo "=== Setting up for GitHub Repository ==="
echo "Repository: https://github.com/amank-23/cpp-trading-engine"
echo ""

# Update the clone URL in README
echo "Updating README with correct repository URL..."
sed -i 's|git clone https://github.com/yourusername/trading-system.git|git clone https://github.com/amank-23/cpp-trading-engine.git|g' README.md
sed -i 's|cd trading-system|cd cpp-trading-engine|g' README.md

echo ""
echo "Steps to complete GitHub setup:"
echo ""
echo "1. Create repository on GitHub:"
echo "   - Go to https://github.com/new"
echo "   - Repository name: cpp-trading-engine"
echo "   - Description: Real-time C++ trading system with ImGui dashboard"
echo "   - Set to Public or Private"
echo "   - Don't initialize with README, .gitignore, or license"
echo ""

read -p "Have you created the GitHub repository? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "2. Adding files to git..."
    git add .
    
    echo "3. Creating initial commit..."
    git commit -m "Initial commit: Real-time C++ trading system

Features:
- Thread-safe order book engine with price-time priority matching
- Real-time WebSocket market data processing
- Risk management with pre-trade controls
- ImGui dashboard for live visualization
- Comprehensive unit testing suite
- Multi-threaded architecture with proper synchronization

Architecture:
- OrderBook: Core matching engine
- WebSocketClient: Market data handler  
- RiskEngine: Position tracking and limits
- Dashboard: Real-time GUI visualization

Built with: C++17, Boost, websocketpp, nlohmann/json, ImGui, OpenGL"
    
    echo "4. Adding GitHub remote..."
    git remote add origin https://github.com/amank-23/cpp-trading-engine.git
    
    echo "5. Setting main branch..."
    git branch -M main
    
    echo "6. Pushing to GitHub..."
    git push -u origin main
    
    echo ""
    echo "✅ Success! Your repository is now on GitHub:"
    echo "   https://github.com/amank-23/cpp-trading-engine"
    echo ""
    echo "Next steps (optional):"
    echo "- Add screenshots to showcase the ImGui dashboard"
    echo "- Create GitHub releases for major versions"
    echo "- Set up GitHub Actions for automated testing"
    
else
    echo ""
    echo "Please create the repository first, then run this script again."
    echo "Repository URL: https://github.com/amank-23/cpp-trading-engine"
fi

echo ""
echo "=== Repository Information ==="
echo "Name: cpp-trading-engine"
echo "Description: Real-time C++ trading system with ImGui dashboard"
echo "Features: Order book engine, WebSocket data, Risk management, GUI dashboard"
echo "Language: C++"
echo "License: MIT"
