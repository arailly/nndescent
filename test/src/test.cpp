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

    auto saved = AKNNG(n, dim, K);
    saved.load(data_path, save_path);

    ASSERT_EQ(
            aknng.edgeset.size(),
            saved.edgeset.size()
            );

    ASSERT_EQ(
            aknng.edgeset[0].size(),
            saved.edgeset[0].size()
    );

    ASSERT_EQ(
            aknng.edgeset[0].begin()->second,
            saved.edgeset[0].begin()->second
    );

    ASSERT_EQ(
            aknng.edgeset[1].begin()->second,
            saved.edgeset[1].begin()->second
    );

    remove(save_path);
}

TEST(aknng, save_load_binary) {
    string data_path = "/mnt/qnap/data/sift/sift_base.fvecs";

    int n = 10, dim = 128, K = 2;
    auto aknng = AKNNG(n, dim, K);
    aknng.build(data_path);

    const char* save_path = "/tmp/sift10-K2.ivecs";
    aknng.save(save_path);

    auto saved = AKNNG(n, dim, K);
    saved.load(data_path, save_path);

    ASSERT_EQ(
            aknng.edgeset.size(),
            saved.edgeset.size()
    );

    ASSERT_EQ(
            aknng.edgeset[0].size(),
            saved.edgeset[0].size()
    );

    ASSERT_EQ(
            aknng.edgeset[0].begin()->second,
            saved.edgeset[0].begin()->second
    );

    ASSERT_EQ(
            aknng.edgeset[0].begin()->first,
            saved.edgeset[0].begin()->first
    );

    ASSERT_EQ(
            aknng.edgeset[1].begin()->second,
            saved.edgeset[1].begin()->second
    );

    ASSERT_EQ(
            aknng.edgeset[1].begin()->first,
            saved.edgeset[1].begin()->first
    );

    remove(save_path);
}
