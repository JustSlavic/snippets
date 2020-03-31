//
// Created by radko on 22.02.19.
//

#pragma once

#include <string>
#include <queue>


class Channel {
public:
    Channel() = default;
    ~Channel() = default;

    void putMessage(const std::string& message);
    std::string readMessage();
protected:
    void applyErrors();

private:
    std::queue<std::string> medium;
};
