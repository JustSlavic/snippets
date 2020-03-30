project(frequencies)

set(CMAKE_CXX_STANDARD 17)

set(COUNTER counter.h counter.cpp)
set(DICTIONARY dictionary.h dictionary.cpp)

add_executable(freq main.cpp ${COUNTER} ${DICTIONARY})
add_executable(test tests.cpp ${COUNTER} ${DICTIONARY})
