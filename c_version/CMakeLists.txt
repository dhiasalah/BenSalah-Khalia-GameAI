cmake_minimum_required(VERSION 3.10)
project(MancalaGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Source files
add_executable(mancala main.cpp)

# Include directories
target_include_directories(mancala PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
