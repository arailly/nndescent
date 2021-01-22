#include <experimental/filesystem>
#include <gtest/gtest.h>
#include <cpputil.hpp>
#include <nndescent.hpp>

using namespace std;
using namespace cpputil;
using namespace nndescent;

TEST(aknng, build) {
    string data_path = "/mnt/qnap/data/sift/sift_base.fvecs";

    int n = 10, dim = 128, K = 2;
    auto aknng = AKNNG(n, dim, K);
    aknng.build(data_path);

    int test_id = 0;
    const auto& neighbors = aknng.edgeset[test_id];
    ASSERT_EQ(neighbors.size(), K);
    ASSERT_EQ(neighbors.cbegin()->second, 2);
    ASSERT_EQ((--neighbors.cend())->second, 6);
}

TEST(aknng, save) {
    string data_path = "/mnt/qnap/data/sift/sift_base.fvecs";

    int n = 10, dim = 128, K = 2;
    auto aknng = AKNNG(n, dim, K);
    aknng.build(data_path);

    const char* save_path = "/tmp/sift10-K2.csv";
    aknng.save(save_path);
    remove(save_path);
}
