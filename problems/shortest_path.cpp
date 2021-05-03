#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include <cassert>

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> r;
    size_t start = 0;
    size_t pos = s.find(delimiter, start);
    while (pos != std::string::npos) {
        r.push_back(s.substr(start, pos - start));
        start = pos + 1;
        pos = s.find(delimiter, start);
    }
    r.push_back(s.substr(start));
    return r;
}

size_t find_min_node(const std::vector<int>& own_distances, const std::vector<bool>& visited) {
    size_t min_index = std::numeric_limits<size_t>::max();
    int min_weight = std::numeric_limits<int>::max();

    for (size_t i = 0; i != own_distances.size(); ++i) {
        if (!visited[i]) {  // can participate
            int curr_weight = own_distances[i];
            if (curr_weight < min_weight) {
                min_weight = curr_weight;
                min_index = i;
            }
        }
    }

    return min_index;
}

std::pair<std::vector<int>, std::vector<size_t>>
find_shortest_path(const std::vector<std::vector<int>>& distance_matrix, size_t start_node) {
    const size_t vertex_count = distance_matrix.size();
    std::vector<int> own_distances(vertex_count, std::numeric_limits<int>::max());
    std::vector<size_t> closest_nodes(vertex_count, std::numeric_limits<size_t>::max());
    std::vector<bool> visited(vertex_count, false);
    own_distances[start_node] = 0;

    for (size_t _ = 0; _ < vertex_count - 1; ++_) {
        const size_t min_node = find_min_node(own_distances, visited);
        visited[min_node] = true;

        const int min_node_distance = own_distances[min_node];
        for (size_t i = 0; i != vertex_count; ++i) {  // find non-visited neighbours and update
            if (visited[i])
                continue;
            const int distance = distance_matrix[min_node][i];
            if (distance == std::numeric_limits<int>::max() || distance == 0)
                continue;
            assert(min_node_distance < std::numeric_limits<int>::max());
            const int new_own_distance = min_node_distance + distance;
            if (new_own_distance < own_distances[i]) {
                own_distances[i] = new_own_distance;
                closest_nodes[i] = min_node;
            }
        }
    }

    return {own_distances, closest_nodes};
}

std::string find_key(const std::unordered_map<std::string, size_t>& m, size_t value) {
    auto it =
        std::find_if(m.begin(), m.end(), [value](const auto& p) { return p.second == value; });
    if (it != m.end())
        return it->first;
    return {};
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: ./shortest_path INPUT_FILE" << std::endl;
        return 1;
    }

    std::fstream file(argv[1], std::ios::in);
    if (!file.is_open()) {
        std::cout << "file is not open: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> distance_matrix;
    // 10 by 10 should be a good start
    distance_matrix.resize(10, std::vector<int>(10, std::numeric_limits<int>::max()));

    std::string first_node;
    std::string last_node;

    // populate data structures
    std::unordered_map<std::string, size_t> vertex_name_to_index;  // setup helper
    // TODO: this doesn't work correctly at all - due to current mapping (which is broken)
    {
        size_t index = 0;
        std::string line;
        // format:
        //      first line: FIRST_NODE LAST_NODE
        //      all others: NODE NODE COST
        std::getline(file, line);
        auto strings = split(line, ' ');
        assert(strings.size() == 2);
        first_node = strings[0];
        last_node = strings[1];

        while (std::getline(file, line)) {
            auto strings = split(line, ' ');
            assert(strings.size() == 3);

            const std::string& i = strings[0];
            const std::string& j = strings[1];
            int weight = std::stoi(strings[2]);

            if (vertex_name_to_index.cend() == vertex_name_to_index.find(i)) {
                vertex_name_to_index[i] = index;
                ++index;
            }

            if (vertex_name_to_index.cend() == vertex_name_to_index.find(j)) {
                vertex_name_to_index[j] = index;
                ++index;
            }

            // fix distance_matrix if it is smaller than needed
            if (index > distance_matrix.size()) {
                for (std::vector<int>& row : distance_matrix)
                    row.resize(index, std::numeric_limits<int>::max());
                distance_matrix.resize(index,
                                       std::vector<int>(index, std::numeric_limits<int>::max()));
            }

            distance_matrix[vertex_name_to_index[i]][vertex_name_to_index[j]] = weight;

            // std::cout << strings[0] << " -> " << strings[1] << ": " << weight << "  // " << line
            //   << std::endl;
        }

        if (index < distance_matrix.size()) {
            for (std::vector<int>& row : distance_matrix) row.resize(index);
            distance_matrix.resize(index);
        }
    }

    distance_matrix = {
        {0, 4, 0, 0, 0, 0, 0, 8, 0},  {4, 0, 8, 0, 0, 0, 0, 11, 0}, {0, 8, 0, 7, 0, 4, 0, 0, 2},
        {0, 0, 7, 0, 9, 14, 0, 0, 0}, {0, 0, 0, 9, 0, 10, 0, 0, 0}, {0, 0, 4, 14, 10, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 2, 0, 1, 6},  {8, 11, 0, 0, 0, 0, 1, 0, 7}, {0, 0, 2, 0, 0, 0, 6, 7, 0}};

    size_t index_of_last = distance_matrix.size() - 1;

    auto [shortest_distances, closest_nodes] = find_shortest_path(distance_matrix, 0);

    std::cout << "own distances:\n";
    std::cout << "vertex \t\t distance\n";
    for (size_t i = 0; i < shortest_distances.size(); ++i) {
        std::cout << std::to_string(i) << " \t\t " << shortest_distances[i] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "full distance from " << first_node << " to " << last_node << ": "
              << shortest_distances[index_of_last] << "\n";
    std::vector<std::string> shortest_path;
    // shortest_path.push_back(last_node);
    size_t current_node = index_of_last;
    while (current_node != 0) {
        // shortest_path.push_back(find_key(vertex_name_to_index, current_node));
        shortest_path.push_back(std::to_string(current_node));
        current_node = closest_nodes[current_node];
    }
    shortest_path.push_back(first_node);
    std::reverse(shortest_path.begin(), shortest_path.end());

    std::cout << "shortest path: ";
    for (size_t i = 0; i < shortest_path.size(); ++i) {
        std::cout << shortest_path[i];
        if (i != shortest_path.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;

    return 0;
}
