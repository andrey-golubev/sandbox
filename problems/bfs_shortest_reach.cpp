#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

struct Node {
    int number;
    vector<int> outs;
    friend bool operator==(const Node& x, const Node& y) { return x.number == y.number; }
    friend bool operator<(const Node& x, const Node& y) { return x.number < y.number; }
};

class Graph {
    vector<Node> m_nodes;

public:
    Graph(int n) {
        m_nodes.resize(n);
        for (int i = 0; i != n; ++i) m_nodes[i].number = i;
    }

    void add_edge(int u, int v) {
        m_nodes[u].outs.push_back(v);
        m_nodes[v].outs.push_back(u);
    }

    vector<int> shortest_reach(int start) {
        vector<int> distances(m_nodes.size(), -1);
        distances[start] = 0;

        list<Node*> nodes;
        nodes.push_back(&m_nodes[start]);

        while (!nodes.empty()) {
            Node* current = nodes.front();
            nodes.pop_front();

            const int current_index = current->number;
            for (int index : current->outs) {
                if (distances[index] == -1) {  // haven't been seen yet
                    nodes.push_back(&m_nodes[index]);
                    distances[index] = distances[current_index] + 6;
                }
            }
        }

        return distances;
    }
};

int main() {
    int queries;
    cin >> queries;

    for (int t = 0; t < queries; t++) {

        int n, m;
        cin >> n;
        // Create a graph of size n where each edge weight is 6:
        Graph graph(n);
        cin >> m;
        // read and set edges
        for (int i = 0; i < m; i++) {
            int u, v;
            cin >> u >> v;
            u--, v--;
            // add each edge to the graph
            graph.add_edge(u, v);
        }
        int startId;
        cin >> startId;
        startId--;
        // Find shortest reach from node s
        vector<int> distances = graph.shortest_reach(startId);

        for (size_t i = 0; i < distances.size(); i++) {
            if (i != size_t(startId)) {
                cout << distances[i] << " ";
            }
        }
        cout << endl;
    }

    return 0;
}
