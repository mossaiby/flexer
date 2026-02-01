cmake_minimum_required(VERSION 3.20)
project(flexer CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(flexer_demo main.cpp)
target_include_directories(flexer_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})