#!/bin/bash

echo "Setting up Trading System development environment..."

# Update package lists
echo "Updating package lists..."
sudo apt update

# Install essential C++ development tools
echo "Installing build tools..."
sudo apt install -y build-essential cmake pkg-config git

# Install required system libraries
echo "Installing system dependencies..."
sudo apt install -y libglfw3-dev libwebsocketpp-dev nlohmann-json3-dev libgtest-dev libgmock-dev libboost-all-dev

# Install OpenGL development libraries
echo "Installing OpenGL libraries..."
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev

# Build and install GTest if needed
echo "Setting up GTest..."
if [ ! -f /usr/lib/x86_64-linux-gnu/libgtest.a ]; then
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp lib/*.a /usr/lib
    cd -
fi

# Initialize git repository if not already done
if [ ! -d ".git" ]; then
    echo "Initializing git repository..."
    git init
fi

# Add ImGui as a submodule
if [ ! -d "third_party/imgui" ]; then
    echo "Adding ImGui as a submodule..."
    mkdir -p third_party
    git submodule add https://github.com/ocornut/imgui.git third_party/imgui
else
    echo "ImGui submodule already exists, updating..."
    git submodule update --init --recursive
fi

echo "Setup complete!"
echo ""
echo "Dependencies installed:"
echo "✓ Build tools (gcc, cmake, pkg-config)"
echo "✓ GLFW3 (GUI windowing)"
echo "✓ WebSocketPP (WebSocket support)"  
echo "✓ nlohmann/json (JSON parsing)"
echo "✓ Google Test (testing framework)"
echo "✓ OpenGL (graphics)"
echo "✓ ImGui (GUI framework)"
echo ""
echo "To build the project:"
echo "  ./build.sh"
echo ""
echo "Or manually:"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  make"
