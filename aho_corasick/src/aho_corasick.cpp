#include <iostream>
#include <queue>
#include <algorithm>
#include "aho_corasick.h"

using namespace aho_corasick;

parser::trie::trie()
    : parent(1, 0)
    , suffix_1(1, '\0')
    , failure(1, 0)
    , terminal(1, false) {

    parent.reserve(PRE_ALLOCATE_N);
    suffix_1.reserve(PRE_ALLOCATE_N);
    failure.reserve(PRE_ALLOCATE_N);
    terminal.reserve(PRE_ALLOCATE_N);
}

/// This function could be implemented by matrix g[nodes][characters]
parser::trie::node parser::trie::go_to(node parent_, char suffix_) {
    for (size_t i = 0; i < size; ++i) {
        if (parent[i] == parent_ && suffix_1[i] == suffix_) return i;
    }
    return 0;
}

parser::trie::node parser::trie::add_node(node parent_, char suffix_) {
    parent.push_back(parent_);
    suffix_1.push_back(suffix_);
    failure.push_back(0);
    terminal.push_back(false);
    size++;

    return size - 1;
}

std::string parser::trie::collect(parser::trie::node prefix) {
    std::string result;

    while (prefix) {
        result.push_back(suffix_1[prefix]);
        prefix = parent[prefix];
    }

    std::reverse(result.begin(), result.end());
    return result;
}

void parser::consume_dictionary(const std::vector<std::string>& dict) {
    // build m_trie
    for (const auto& word : dict) {
        trie::node current = 0;

        for (auto c : word) {
            trie::node found = m_trie.go_to(current, c);
            current = found ? found : m_trie.add_node(current, c);
        }

        m_trie.terminal[current] = true;
    }

    // use BFS to build failure functions
    std::queue<trie::node> q;

    // the first layer goes into queue (root is 0th layer)
    for (trie::node i = 1; i < m_trie.size; ++i) {
        if (m_trie.parent[i] == trie::root) {
            q.push(i);
        }
    }

    while (!q.empty()) {
        trie::node current = q.front(); q.pop();

        // traverse along failure link from current node
        trie::node f = m_trie.failure[current];

        // try to find links from node 'f' with the same letter as 'next_node' links to current node,
        // if such found => this is where failure link of 'next_node' should go
        // if not found => point failure link to the root
        for (trie::node next_node = 0; next_node < m_trie.size; ++next_node) {
            if (m_trie.parent[next_node] != current) continue;

            trie::node found = m_trie.go_to(f, m_trie.suffix_1[next_node]);
            m_trie.failure[next_node] = found;
            q.push(next_node);
        }
    }
}

parser::trie::node parser::next_node(parser::trie::node current, char c) {
    while (true) {
        trie::node found = m_trie.go_to(current, c);

        if (found) return found;

        current = m_trie.failure[current];
        if (current == trie::root) return trie::root;
    }
}

