#include <fstream>

#include "dictionary.h"
#include "counter.h"


void push_output(std::ostream& output, const std::vector<dictionary::value_t>& data) {
    for (auto&& pair : data) {
        output << pair.first << " - " << pair.second << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "usage: freq <input file> <output file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Cannot open the input file";
        return EXIT_FAILURE;
    }

    std::ofstream output(argv[2]);
    if (!output.is_open()) {
        std::cerr << "Cannot open the output file";
        return EXIT_FAILURE;
    }

    auto result = count_words(input);

    push_output(output, result);

    return EXIT_SUCCESS;
}
