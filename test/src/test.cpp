#include <gtest/gtest.h>
#include <arailib.hpp>
#include <aknng.hpp>

using namespace std;
using namespace arailib;
using namespace aknng;

TEST(aknng, build) {
    string base_dir = "/tmp/tmp.aknng/test/src/";
    string data_path = base_dir + "data1.csv";

    int degree = 3;
    auto aknng = AKNNG(degree);
    aknng.build(data_path);

    ASSERT_EQ(aknng.nodes.size(), 11);
}