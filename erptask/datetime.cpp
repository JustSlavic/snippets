//
// Created by radko on 12/14/16.
//

#include "datetime.h"

bool operator==(const DateTime& lhs, const DateTime& rhs) {
    return lhs.compare(rhs) == 0;
}

bool operator!=(const DateTime& lhs, const DateTime& rhs) {
    return !(lhs == rhs);
}

bool operator<(const DateTime& lhs, const DateTime& rhs) {
    return lhs.compare(rhs) < 0;
}

bool operator<=(const DateTime& lhs, const DateTime& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

bool operator>=(const DateTime& lhs, const DateTime& rhs) {
    return !(lhs < rhs);
}

bool operator>(const DateTime& lhs, const DateTime& rhs) {
    return !(lhs <= rhs);
}

std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
    string dts = static_cast<string>(dt);
    os << dts;
    return os;
}

DateTime::DateTime(string s) {
    s += ' ';
    int i = 0;
    int b = 0;
    for (auto&& c : s) {
        if (c==' ' || c=='.' || c==':') {
            ++i;

            switch (i) {
            case 1:
                dt.tm_mday = b;
                break;
            case 2:
                dt.tm_mon = b-1;
                break;
            case 3:
                dt.tm_year = b-1900;
                break;
            case 4:
                dt.tm_hour = b;
                break;
            case 5:
                dt.tm_min = b;
                break;
            default:
                break;
            }

            b = 0;
        }
        else {
            b = b*10+c-'0';
        }
    }

    t = mktime(&dt);
}

DateTime::operator string() const {
    string s = (dt.tm_mday   >9 ? to_string(dt.tm_mday)  : '0' + to_string(dt.tm_mday)) + '.' +
            ((dt.tm_mon+1)>9 ? to_string(dt.tm_mon+1) : '0' + to_string(dt.tm_mon+1)) + '.' +
            to_string(dt.tm_year + 1900) + ' ' +
            (dt.tm_hour>9    ? to_string(dt.tm_hour)  : '0' + to_string(dt.tm_hour)) + ':' +
            (dt.tm_min>9     ? to_string(dt.tm_min)   : '0' + to_string(dt.tm_min));
    return s;
}

int DateTime::compare(const DateTime& other) const {
    double tmp = difftime(t, other.getTime());
    if (tmp > 0) {
        return 1;
    }
    if (tmp < 0) {
        return -1;
    }
    return 0;
}









