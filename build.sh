#!/bin/bash

echo "Building Trading System..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Change to build directory
cd build

# Run cmake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run the executable with: ./build/TradingSystem"
else
    echo "Build failed!"
    exit 1
fi
