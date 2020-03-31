//
// Created by radko on 22.02.19.
//

#include <Channel.hpp>

void Channel::putMessage(const std::string& message) {
    medium.push(message);
}

std::string Channel::readMessage() {
    if (medium.empty()) {
        return "";
    } else {
        auto&& value = medium.front();
        medium.pop();
        return value;
    }
}

void Channel::applyErrors() {
    auto& m = medium.front();
    auto message_size = m.size();
    //get random i
    size_t i = 0;
    m[i] =
}

