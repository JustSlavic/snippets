#include <unicode/unistr.h>

#include "counter.h"


template <typename Func>
void apply_to_words(std::istream& input, Func callback) {
    std::string buffer;
    buffer.reserve(50);

    while (!input.eof()) {
        char c = input.get();

        if (std::isalpha(c)) {
            buffer.push_back(std::tolower(c));
        } else if (!buffer.empty()) {
            callback(buffer);
            buffer.clear();
        }
    }

    if (!buffer.empty()) {
    
        callback(buffer);
    }
}


std::vector<dictionary::value_t> count_words(std::istream& input) {
    dictionary dict;

    apply_to_words(input, [&dict](std::string& word) { dict.push(word); });

    return dict.to_vector();
}
