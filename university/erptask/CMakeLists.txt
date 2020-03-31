cmake_minimum_required(VERSION 3.5)
project(erptask)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
#set(CMAKE_C_COMPILER "clang-3.8")
#set(CMAKE_CXX_COMPILER "clang++-3.8")

set(SOURCE_FILES main.cpp
        bankDocument.cpp bankDocument.h
        warehouseDocument.cpp warehouseDocument.h
        company.cpp company.h
        parser.cpp parser.h
        document.h document.cpp
        datetime.cpp datetime.h
        bank.cpp bank.h)
add_executable(erptask ${SOURCE_FILES})