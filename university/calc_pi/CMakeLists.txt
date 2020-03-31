cmake_minimum_required(VERSION 3.7)
project(calc_pi)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP)

set(CMAKE_C_FLAGS ${OpenMP_C_FLAGS})

add_executable(pi main.c)
