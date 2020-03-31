//
// Created by radko on 12/14/16.
//

#pragma once

#include <ostream>
#include <ctime>
#include <unistd.h>
#include <string>

using std::string;
using std::to_string;

class DateTime {
public:

    DateTime() = default;

    explicit DateTime(string s);  //format: DD.MM.YYYY HH:MM

    explicit operator string() const;

    time_t getTime() const { return t; }

    int compare(const DateTime& other) const;

protected:
    time_t t = time(0);
    struct tm dt{0, 0, 0, 0, 0, 0, 0, 0, -1, 0, nullptr};
};

bool operator==(const DateTime& lhs, const DateTime& rhs);
bool operator!=(const DateTime& lhs, const DateTime& rhs);
bool operator< (const DateTime& lhs, const DateTime& rhs);
bool operator<=(const DateTime& lhs, const DateTime& rhs);
bool operator>=(const DateTime& lhs, const DateTime& rhs);
bool operator> (const DateTime& lhs, const DateTime& rhs);
std::ostream& operator<<(std::ostream& os, const DateTime& dt);

