#include <gtest/gtest.h>
#include <cpputil.hpp>
#include <nndescent.hpp>

using namespace std;
using namespace cpputil;
using namespace nndescent;

string base_dir = "/tmp/tmp.dH0F0yrqaE/test/src/";

TEST(aknng, get_neighbors_list) {
    string data_path = base_dir + "data1.csv";

    int degree = 3;
    auto aknng = AKNNG(degree);
    aknng.build(data_path);

    const auto neighbors_list = aknng.get_neighbors_list();

    int test_id = 0;
    for (const auto neighbor_id : neighbors_list[test_id]) {
        bool include_neighbor = false;

        for (const auto neighbor_pair : aknng.nodes[test_id].neighbors) {
            if (neighbor_id != neighbor_pair.second) continue;
            include_neighbor = true;
            break;
        }

        for (const auto reverse_neighbor_pair : aknng.nodes[neighbor_id].neighbors) {
            if (test_id != reverse_neighbor_pair.second) continue;
            include_neighbor = true;
            break;
        }

        ASSERT_TRUE(include_neighbor);
    }
}

TEST(aknng, build) {
    string data_path = base_dir + "data1.csv";

    int degree = 3;
    auto aknng = AKNNG(degree);
    aknng.build(data_path);

    int test_id = 0;
    const auto& neighbors = aknng.nodes[test_id].neighbors;
    ASSERT_EQ(neighbors.cbegin()->second, 3);
    ASSERT_EQ((--neighbors.cend())->second, 2);
}

TEST(aknng, save) {
    string data_path = base_dir + "data1.csv";

    int degree = 3;
    auto aknng = AKNNG(degree);
    aknng.build(data_path);

    string save_path = base_dir + "graph1.csv";
    aknng.save(save_path);
}

TEST(aknng, build_angular) {
    string data_path = base_dir + "data1.csv";

    int degree = 2;
    auto aknng = AKNNG(degree, "angular");
    aknng.build(data_path);

    int test_id = 3;
    const auto& neighbors = aknng.nodes[test_id].neighbors;
    ASSERT_EQ(neighbors.cbegin()->second, 10);
}