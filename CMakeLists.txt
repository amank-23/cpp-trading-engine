cmake_minimum_required(VERSION 3.16)
project(TradingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# --- Project Source Files ---
# Get all sources except main.cpp for the library
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Create a library with our core logic
add_library(TradingSystemLib 
    ${LIB_SOURCES}
    # --- Add ImGui source files ---
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)

# Create the main executable
add_executable(TradingSystem src/main.cpp)

# Create a backend test executable
add_executable(BackendTest tests/test_order_book.cpp)

# --- Find Required Packages ---
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# --- Find Optional Packages ---
find_package(OpenGL QUIET)
find_package(glfw3 QUIET)
find_package(GTest QUIET)

# --- Include Directories ---
target_include_directories(TradingSystemLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(TradingSystem PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(BackendTest PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Conditionally add ImGui directories if available
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
    target_include_directories(TradingSystemLib PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends"
    )
    target_include_directories(TradingSystem PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends"
    )
    set(IMGUI_AVAILABLE ON)
    message(STATUS "ImGui found and will be included")
endif()

# Conditionally add websocketpp if available
if(EXISTS "/usr/include/websocketpp")
    target_include_directories(TradingSystemLib PUBLIC /usr/include)
    target_include_directories(TradingSystem PUBLIC /usr/include)
    set(WEBSOCKETPP_AVAILABLE ON)
    message(STATUS "websocketpp found and will be included")
endif()

# Conditionally add nlohmann/json if available
if(EXISTS "/usr/include/nlohmann")
    target_include_directories(TradingSystemLib PUBLIC /usr/include/nlohmann)
    target_include_directories(TradingSystem PUBLIC /usr/include/nlohmann)
    set(NLOHMANN_JSON_AVAILABLE ON)
    message(STATUS "nlohmann/json found and will be included")
endif()

# --- Link Libraries ---
# Link libraries to the core library
target_link_libraries(TradingSystemLib PRIVATE 
    Threads::Threads
    ${Boost_LIBRARIES}
)

# Link the main executable to the library
target_link_libraries(TradingSystem PRIVATE TradingSystemLib)

# Link the backend test to the library
target_link_libraries(BackendTest PRIVATE TradingSystemLib)

# Link optional libraries if found
if(OpenGL_FOUND)
    target_link_libraries(TradingSystemLib PRIVATE OpenGL::GL)
    target_link_libraries(TradingSystem PRIVATE OpenGL::GL)
    message(STATUS "OpenGL found and linked")
endif()

if(glfw3_FOUND)
    target_link_libraries(TradingSystemLib PRIVATE glfw)
    target_link_libraries(TradingSystem PRIVATE glfw)
    message(STATUS "GLFW3 found and linked")
endif()

if(GTest_FOUND)
    target_link_libraries(TradingSystemLib PRIVATE GTest::gtest GTest::gmock)
    message(STATUS "Google Test found and linked")
endif()

# --- Compiler Flags ---
target_compile_options(TradingSystemLib PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(TradingSystem PRIVATE -Wall -Wextra -Wpedantic)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(TradingSystemLib PRIVATE -O3)
    target_compile_options(TradingSystem PRIVATE -O3)
endif()

# Add preprocessor definitions based on available libraries
if(IMGUI_AVAILABLE)
    target_compile_definitions(TradingSystemLib PRIVATE HAS_IMGUI=1)
    target_compile_definitions(TradingSystem PRIVATE HAS_IMGUI=1)
endif()

if(WEBSOCKETPP_AVAILABLE)
    target_compile_definitions(TradingSystemLib PRIVATE HAS_WEBSOCKETPP=1)
    target_compile_definitions(TradingSystem PRIVATE HAS_WEBSOCKETPP=1)
endif()

if(NLOHMANN_JSON_AVAILABLE)
    target_compile_definitions(TradingSystemLib PRIVATE HAS_NLOHMANN_JSON=1)
    target_compile_definitions(TradingSystem PRIVATE HAS_NLOHMANN_JSON=1)
endif()

if(OpenGL_FOUND AND glfw3_FOUND)
    target_compile_definitions(TradingSystemLib PRIVATE HAS_GUI=1)
    target_compile_definitions(TradingSystem PRIVATE HAS_GUI=1)
endif()

# --- Enable Testing ---
enable_testing()

# --- Test Configuration ---
if(GTest_FOUND)
    # Create test executable
    add_executable(RunTests tests/test_order_book.cpp)

    # Link the test executable against our library and GTest
    target_link_libraries(RunTests PRIVATE
        TradingSystemLib
        GTest::gtest
        GTest::gmock
        GTest::gtest_main # This provides a main() for the test runner
    )

    # Add the test to CTest so 'make test' works
    include(GoogleTest)
    gtest_add_tests(TARGET RunTests)
    
    message(STATUS "Tests configured - use 'make test' or './RunTests' to run")
endif()

# --- Status Report ---
message(STATUS "=== TradingSystem Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL: ${OpenGL_FOUND}")
message(STATUS "GLFW3: ${glfw3_FOUND}")
message(STATUS "Google Test: ${GTest_FOUND}")
message(STATUS "ImGui: ${IMGUI_AVAILABLE}")
message(STATUS "websocketpp: ${WEBSOCKETPP_AVAILABLE}")
message(STATUS "nlohmann/json: ${NLOHMANN_JSON_AVAILABLE}")
message(STATUS "Boost: ${Boost_FOUND}")
