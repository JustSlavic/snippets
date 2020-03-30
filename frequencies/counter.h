#pragma once

#include <functional>
#include <vector>
#include <iostream>

#include "dictionary.h"

/*
 *  applies callback to each word in given stream
 */ 
void apply_to_words(std::istream& input, const std::function<void(std::string&)>& callback);

/*
 *  count words in given stream resulting sorted vector of pairs (word, count)
 */
std::vector<std::pair<std::string, size_t>> count_words(std::istream& input);
