cmake_minimum_required(VERSION 3.20)
project(flexer CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)

add_executable(demo demo.cpp)
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})