#include <cassert>
#include <iostream>
#include <fstream>

#include "dictionary.h"
#include "counter.h"


void test_dict() {
    dictionary dict;

    dict.push("word");
    dict.push("mice");
    dict.push("make");
    dict.push("mice");

    std::vector<dictionary::value_t> expected {
        {"mice", 2},
        {"make", 1},
        {"word", 1}
    };

    assert(dict.to_vector() == expected);
}


void test_file(
    const std::string& filename, 
    const std::vector<dictionary::value_t>& expected,
    const std::string msg = "") 
{
    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Cannot open the input file";
        std::exit(1);
    }

    auto result = count_words(input);

    if (result != expected) {
        std::cerr << msg << " failed:\nexpected: " << expected << "\noutput: " << result << std::endl;
        std::exit(1);
    } else {
        std::cout << msg << " success" << std::endl;
    }
}


int main() {
    test_dict();
    std::cout << "Test dictionary 1 SUCCESS" << std::endl;

    test_file("tests/test1.txt", {{"cat", 8}}, "test1");
    test_file("tests/test2.txt", {{"cat", 2}, {"the", 2}, {"jumped", 1}, {"on", 1}}, "test2");
    test_file("tests/test3.txt", {}, "test3");
    test_file("tests/test4.txt", {}, "test4");

    return EXIT_SUCCESS;
}
