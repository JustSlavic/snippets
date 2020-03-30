#pragma once

#include <vector>
#include <map>
#include <functional>
#include <ostream>


/*
 *  adapter to map that encapsulates custom sorting function when we get vector from it
 */
struct dictionary {
    using value_t = std::pair<std::string, size_t>;
    std::map<std::string, size_t> container;

    void push(const std::string& value);
    std::vector<value_t> to_vector() const;
};

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
    return os << "(" << p.first << ", " << p.second << ")";
}

template <typename T, typename U, template<typename P> typename C>
std::ostream& operator<<(std::ostream& os, const C<std::pair<T, U>>& container) {
    if (container.empty()) return os << "[]";
    auto it = container.begin();
    os << "[" << *it++;
    for (; it != container.end(); ++it) { os << ", " << *it; }
    return os << "]";
}
