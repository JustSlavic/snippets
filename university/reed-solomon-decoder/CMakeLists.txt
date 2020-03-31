cmake_minimum_required(VERSION 3.12)
project(reed_solomon_decoder)

set(CMAKE_CXX_STANDARD 14)

find_library(NTL_LIBRARY ntl)

include_directories(include)
link_directories(lib)

set(MAIN src/main.cpp)
set(CODE include/ReedSolomonCode.hpp src/ReedSolomonCode.cpp)
set(CHANNEL include/Channel.hpp src/Channel.cpp)
set(TRANSMITTER include/Transmitter.hpp src/Transmitter.cpp)
set(RECIEVER include/Reciever.hpp src/Reciever.cpp)

add_executable(decoder ${MAIN} ${CODE})
target_link_libraries(decoder "${NTL_LIBRARY}" Logging)
