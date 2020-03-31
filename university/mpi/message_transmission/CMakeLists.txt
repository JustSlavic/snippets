cmake_minimum_required(VERSION 3.7)
project(supercomputers)

set(CMAKE_C_COMPILER /home/radko/.local/bin/mpicc)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP)

set(CMAKE_C_FLAGS ${OpenMP_C_FLAGS})

add_executable(task4 main.c)