#include <iostream>
#include <fstream>
#include <sstream>
#include "src/aho_corasick.h"


std::string read_whole_file(const char *filename) {
    std::ifstream input(filename, std::ios::in | std::ios::binary);
    std::ostringstream content;

    if (input.good()) {
        content << input.rdbuf();
    }

    return content.str();
}

int main() {
    std::ifstream input("test/dict_001.txt");
    std::vector<std::string> dictionary;
    std::string buf;

    while (getline(input, buf)) {
        dictionary.push_back(std::move(buf));
    }

    std::string text = read_whole_file("test/text_001.txt");

    aho_corasick::parser parser;
    parser.consume_dictionary(dictionary);
    std::cout << "===== parse ======\n";
    parser.parse(text, [](const std::string &word) { std::cout << "found " << word << std::endl; });
    std::cout << "=== full words ===\n";
    parser.parse_full_words(text, [](const std::string &word) { std::cout << "found " << word << std::endl; });
    std::cout << "==================\n";

    return 0;
}
