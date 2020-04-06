#pragma once

#include <string>
#include <vector>

namespace aho_corasick {

// todo make this parser consume all types of characters and even Unicode (template ???)
class parser {
public:
    void consume_dictionary(const std::vector<std::string>& dict);

    template <typename Fn>
    void parse(const std::string& text, Fn callback);

    template <typename Fn>
    void parse_full_words(const std::string& text, Fn callback);

private:
    struct trie {
        using node = size_t;
        static const node root = 0;
        const size_t PRE_ALLOCATE_N = 100;

        std::vector<node> parent;
        std::vector<char> suffix_1; // suffix of length 1
        std::vector<node> failure;
        std::vector<bool> terminal;

        size_t size = 1;

        trie();
        node go_to(node parent_, char suffix_);
        node add_node(node parent_, char suffix_);
        std::string collect(node prefix);
    };

    trie::node next_node(trie::node, char);

    trie m_trie;
};


template<typename Fn>
void parser::parse(const std::string &text, Fn callback) {
    std::string::const_iterator it = text.begin();
    trie::node current = 0;

    while (it != text.end()) {
        char c = *it++; // eat character

        current = next_node(current, c);

        // print as many thing as possible
        trie::node t = current; // make copy of current node
        while (t != trie::root) { // traverse through failure links until reaching root node
            if (m_trie.terminal[t]) {
                callback(m_trie.collect(t)); // call function on all terminals along the way
            }
            t = m_trie.failure[t];
        }
    }
}

template<typename Fn>
void parser::parse_full_words(const std::string &text, Fn callback) {
    std::string::const_iterator it = text.begin();
    trie::node current = 0;

    while (it != text.end()) {
        char c = *it++; // eat character

        while (true) {
            trie::node found = m_trie.go_to(current, c);

            if (found) { // if we found the thing
                current = found;
                break;
            } else if (m_trie.terminal[current]) { // if failed to go further in trie, but this is terminal => we found our word
                callback(m_trie.collect(current));
                current = trie::root;
            } else { // if failed to go further in trie, but this is not terminal => go to failure link and keep search
                current = m_trie.failure[current];
                break;
            }
        }
    }
}

} // aho_corasick
