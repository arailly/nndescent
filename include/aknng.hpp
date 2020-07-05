//
// Created by Yusuke Arai on 2020/07/05.
//

#ifndef NNDESCENT_AKNNG_HPP
#define NNDESCENT_AKNNG_HPP

#include <arailib.hpp>

using namespace std;
using namespace arailib;

namespace aknng {
    struct Node {
        int id;
        Data<> data;
        int degree;
        multimap<double, int> neighbors;
        unordered_map<size_t, bool> added;

        Node(const Data<>& data, int degree) : data(data), id(data.id), degree(degree) {
            added[data.id] = true;
        }

        int add_neighbor(const Node& node) {
            if (added.find(node.id) != added.end()) return 0;
            added[node.id] = true;
            const auto dist = euclidean_distance(data, node.data);
            neighbors.emplace(dist, node.id);

            // delete neighbor if over degree
            if (neighbors.size() > degree) {
                const auto tail_ptr = --neighbors.cend();
                neighbors.erase(tail_ptr);
                added.erase(tail_ptr->second);
            }

            return 1;
        }

        auto get_n_neighbors() const { return neighbors.size(); }
    };

    struct AKNNG {
        vector<Node> nodes;
        int degree;
        mt19937 engine;

        AKNNG(int degree) : degree(degree), engine(42) {}
        auto size() const { return nodes.size(); }
        auto begin() const { return nodes.begin(); }
        auto end() const { return nodes.end(); }
        decltype(auto) operator [] (size_t i) { return nodes[i]; }
        decltype(auto) operator [] (const Node& n) { return nodes[n.id]; }

        auto get_neighbors_list() {
            vector<vector<int>> neighbors_list(nodes.size());
            unordered_map<int, bool> added;

            for (const auto& node : nodes) {
                for (const auto& neighbor_pair : node.neighbors) {
                    const auto neighbor_id = neighbor_pair.second;

                    // get neighbors
                    neighbors_list[node.id].emplace_back(neighbor_id);

                    // get reverse neighbors
                    neighbors_list[neighbor_id].emplace_back(node.id);
                }
            }

            return neighbors_list;
        }

        void build(string data_path, int n = -1) {
            const auto dataset = load_data(data_path, n);

            // init nodes
            for (auto& data : dataset) {
                nodes.emplace_back(move(data), degree);
            }

            // init neighbors
            uniform_int_distribution<int> dist(0, nodes.size() - 1);
            for (auto& node : nodes) {
                while (node.neighbors.size() < degree) {
                    const auto& random_node = nodes[dist(engine)];
                    node.add_neighbor(random_node);
                }
            }
        }
    };
}

#endif //NNDESCENT_AKNNG_HPP
