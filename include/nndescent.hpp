//
// Created by Yusuke Arai on 2020/07/05.
//

#ifndef NNDESCENT_NNDESCENT_HPP
#define NNDESCENT_NNDESCENT_HPP

#include <cpputil.hpp>
#include <random>

using namespace std;
using namespace cpputil;

namespace nndescent {
    struct Node {
        int id;
        Data<> data;
        int degree;
        multimap<double, int> neighbors;
        unordered_map<size_t, bool> added;
        DistanceFunction<> df;

        Node(const Data<>& data, int degree,
             DistanceFunction<> df):
                data(data), id(data.id), degree(degree), df(df) {
            added[data.id] = true;
        }

        int add_neighbor(const Node& node) {
            if (added.find(node.id) != added.end()) return 0;

            const auto dist = df(data, node.data);

            if (neighbors.size() < degree) {
                neighbors.emplace(dist, node.id);
                added[node.id] = true;
                return 1;
            }

            const auto furthest_ptr = --neighbors.cend();
            const auto furthest_dist = furthest_ptr->first;

            if (dist >= furthest_dist) return 0;

            neighbors.emplace(dist, node.id);
            added[node.id] = true;

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
        DistanceFunction<> df;

        AKNNG(int degree, string dist_kind = "euclidean") :
                degree(degree), df(select_distance(dist_kind)),
                engine(42) {}

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

        void build(const Dataset<>& dataset) {
            // init nodes
            for (auto& data : dataset) {
                nodes.emplace_back(move(data), degree, df);
            }

            // init neighbors
            uniform_int_distribution<int> dist(0, nodes.size() - 1);
            for (auto& node : nodes) {
                while (node.neighbors.size() < degree) {
                    const auto& random_node = nodes[dist(engine)];
                    node.add_neighbor(random_node);
                }
            }

            auto n_itr = 0;
            while (true) {
                long long int n_updated = 0;
                const auto neighbors_list = get_neighbors_list();
#pragma omp parallel
                {
#pragma omp for schedule(dynamic, 1000) nowait reduction(+:n_updated)
                    for (int i = 0; i < nodes.size(); ++i) {
                        auto& node = nodes[i];

                        for (const auto neighbor_id_1 : neighbors_list[node.id]) {
                            for (const auto neighbor_id_2 : neighbors_list[neighbor_id_1]) {
                                const auto& neighbor = nodes[neighbor_id_2];
                                n_updated += node.add_neighbor(neighbor);
                            }
                        }
                    }
                };
                cout << "iteration: " << n_itr << ", update: " << n_updated << endl;
                if (n_updated <= 0) break;
                ++n_itr;
            }
        }

        void build(string data_path, int n = -1) {
            build(load_data(data_path, n));
        }

        void save(const string& save_path) {
            // csv
            if (is_csv(save_path)) {
                ofstream ofs(save_path);
                string line;
                for (const auto& node : nodes) {
                    for (const auto& neighbor_pair : node.neighbors) {
                        line = to_string(node.data.id) + ',' +
                               to_string(neighbor_pair.second) + ',' +
                               to_string(neighbor_pair.first);
                        ofs << line << endl;
                    }
                }
                return;
            }

            // dir
            vector<string> lines(static_cast<unsigned long>(ceil(nodes.size() / 1000.0)));
            for (const auto& node : nodes) {
                const size_t line_i = node.data.id / 1000;
                for (const auto& neighbor_pair : node.neighbors) {
                    lines[line_i] += to_string(node.data.id) + "," +
                                     to_string(neighbor_pair.second) + "," +
                                     to_string(neighbor_pair.first) + "\n";
                }
            }

            for (int i = 0; i < lines.size(); i++) {
                const string path = save_path + "/" + to_string(i) + ".csv";
                ofstream ofs(path);
                ofs << lines[i];
            }
        }
    };
}

#endif //NNDESCENT_NNDESCENT_HPP
