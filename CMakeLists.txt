# CMakeLists.txt for a basic C++ project

cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(FalcoEngine VERSION 0.1)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(FalcoEngine main.cpp)  # Change main.cpp to your source files

# Include directories
include_directories(include)  # If you have an include directory for headers