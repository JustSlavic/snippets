cmake_minimum_required(VERSION 3.5)
project(rk4_particles C)

set(CMAKE_C_COMPILER /home/radko/.local/bin/mpicc)
set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)

add_executable(rk4_particles ${SOURCE_FILES})