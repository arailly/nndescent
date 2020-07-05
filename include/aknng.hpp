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

            if (neighbors.size() < degree) {
                neighbors.emplace(dist, node.id);
                return 1;
            }

            const auto furthest_ptr = --neighbors.cend();
            const auto furthest_dist = furthest_ptr->first;

            if (dist > furthest_dist) return 0;

            neighbors.emplace(dist, node.id);

            // delete neighbor if over degree
            if (neighbors.size() > degree) {
                neighbors.erase(furthest_ptr);
                added.erase(furthest_ptr->second);
            }

            return 1;
        }
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

            while (true) {
                int n_updated = 0;
                const auto neighbors_list = get_neighbors_list();
                for (int id = 0; id < nodes.size(); ++id) {
                    auto& node = nodes[id];

                    for (const auto neighbor_id_1 : neighbors_list[id]) {
                        for (const auto neighbor_id_2 : neighbors_list[neighbor_id_1]) {
                            const auto& neighbor = nodes[neighbor_id_2];
                            n_updated += node.add_neighbor(neighbor);
                        }
                    }
                }

                if (n_updated <= 0) break;
            }
        }
    };
}

#endif //NNDESCENT_AKNNG_HPP
