#include "dictionary.h"


void dictionary::push(const std::string& value) {
    container[value]++;
}

std::vector<dictionary::value_t> dictionary::to_vector() const {
    std::vector<value_t> result;
    for (auto&& pair : container) {
        result.emplace_back(pair);
    }

    std::sort(result.begin(), result.end(), [](auto& lhs, auto& rhs) {
        return lhs.second == rhs.second ? lhs.first < rhs.first : lhs.second > rhs.second;
    });

    return result;
}
