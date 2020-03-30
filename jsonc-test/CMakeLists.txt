cmake_minimum_required(VERSION 3.14)
project(jsonc-test)

set(CMAKE_CXX_STANDARD 14)

find_library(json-c REQUIRED)
find_library(readline REQUIRED)

add_executable(jsonc-test main.cpp)

target_link_libraries(jsonc-test json-c)
target_link_libraries(jsonc-test readline)

