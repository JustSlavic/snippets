cmake_minimum_required(VERSION 3.5)
project(rk4_particles C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)

add_executable(rk4_particles ${SOURCE_FILES})

target_link_libraries(rk4_particles m)