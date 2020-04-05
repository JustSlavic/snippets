#pragma once

#include <string>
#include <vector>

namespace aho_corasick {

class parser {
public:
    void consume_dictionary(const std::vector<std::string>& dict);

    template <typename Fn>
    void parse(const std::string& text, Fn callback) {
        std::string::const_iterator it = text.begin();
        trie::node current_node = 0;

        while (it != text.end()) {
            char c = *it++; // eat character

            current_node = next_node(current_node, c);

            // print as many thing as possible
            trie::node t = current_node;
            while (t != trie::root) {
                if (m_trie.terminal[t]) {
                    callback(m_trie.collect(t));
                }
                t = m_trie.failure[t];
            }
        }
    }

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

} // aho_corasick
