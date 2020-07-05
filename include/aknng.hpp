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

//        void add_neighbor(int node_id) {
//            if (added.find(node_id) != added.end()) return;
//            added[node_id] = true;
//            neighbors.push_back(node_id);
//        }

        auto get_n_neighbors() const { return neighbors.size(); }
    };

    struct AKNNG {
        vector<Node> nodes;
        int degree;

        AKNNG(int degree) : degree(degree) {}
        auto size() const { return nodes.size(); }
        auto begin() const { return nodes.begin(); }
        auto end() const { return nodes.end(); }
        decltype(auto) operator [] (size_t i) { return nodes[i]; }
        decltype(auto) operator [] (const Node& n) { return nodes[n.id]; }
    };
}

#endif //NNDESCENT_AKNNG_HPP
