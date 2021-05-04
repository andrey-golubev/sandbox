#include <bits/stdc++.h>

using namespace std;

struct Node {
    string value;
    unordered_map<char, vector<size_t>> children_by_link;
    bool terminal = false;
};

struct node_hasher {
    using T = Node*;
    size_t operator()(const T& n) const { return std::hash(n->value); }
};

struct Trie {
    vector<Node> m_nodes;

    Trie() { m_nodes.push_back(Node{}); }

    bool contains(const Node& n, char c) {
        return n.children_by_link.cend() != n.children_by_link.find(c);
    }

    void insert(const string& value) {
        size_t current = 0;

        // string_view would've been ideal here?
        for (size_t i = 0; i < value.size(); ++i) {
            char link_symbol = value[i];

            if (!contains(m_nodes[current], link_symbol)) {
                // create new node
                Node n{};
                n.value = value.substr(0, i + 1);
                m_nodes.push_back(std::move(n));
                m_nodes[current].children_by_link[link_symbol].push_back(m_nodes.size() - 1);
            }

            current = m_nodes[current].children_by_link[link_symbol].back();
        }
    }

    Node* find(const string& value) {
        Node* current = &m_nodes.front();

        for (size_t i = 0; i < value.size(); ++i) {
            char link_symbol = value[i];
            if (!contains(*current, link_symbol)) {
                return nullptr;
            }
            current = &m_nodes[current->children_by_link[link_symbol]];
        }

        return current;
    }

    unordered_set<Node*, node_hasher> collect_leaves(Node* n) {
        unordered_set<Node*, node_hasher> leaves;
        if (!n) {
            return leaves;
        }

        list<Node*> nodes;
        nodes.push_back(n);
        while (!nodes.empty()) {
            Node* current = nodes.front();
            nodes.pop_front();

            if (current->children_by_link.empty()) {
                leaves.insert(current);
                continue;
            }

            for (const auto& p : current->children_by_link) {
                for (const auto& index : p.second) {
                    nodes.push_back(&m_nodes[index]);
                }
            }
        }

        return leaves;
    }
};

/*
 * Complete the contacts function below.
 */
vector<int> contacts(vector<vector<string>> queries) {
    /*
     * Write your code here.
     */
    vector<int> ret;
    Trie t;
    for (const auto& query : queries) {
        if (query[0] == std::string("add")) {
            // build a trie
            t.insert(query[1]);
        } else {  // find
            assert(query[0] == std::string("find"));
            // find in a trie
            Node* match = t.find(query[1]);
            auto leaves = t.collect_leaves(match);
            ret.push_back(int(leaves.size()));
        }
    }

    return ret;
}

int main() {
    int queries_rows;
    cin >> queries_rows;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<vector<string>> queries(queries_rows);
    for (int queries_row_itr = 0; queries_row_itr < queries_rows; queries_row_itr++) {
        queries[queries_row_itr].resize(2);

        for (int queries_column_itr = 0; queries_column_itr < 2; queries_column_itr++) {
            cin >> queries[queries_row_itr][queries_column_itr];
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    vector<int> result = contacts(queries);

    for (size_t result_itr = 0; result_itr < result.size(); result_itr++) {
        cout << result[result_itr];

        if (result_itr != result.size() - 1) {
            cout << "\n";
        }
    }

    cout << "\n";

    return 0;
}
