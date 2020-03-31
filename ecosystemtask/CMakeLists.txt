cmake_minimum_required(VERSION 3.5)
project(ecosystemtask)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp animals.h animals.cpp events.h model.h model.cpp events.cpp parse.cpp parse.h)

add_executable(ecosystemtask ${SOURCE_FILES})