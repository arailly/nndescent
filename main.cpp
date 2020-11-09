#include <arailib.hpp>
#include <aknng.hpp>

using namespace std;
using namespace arailib;
using namespace aknng;

int main() {
    string base_dir = "/home/arai/workspace/";
    string data_path = base_dir + "dataset/sift/data1m/";
    string distance = "euclidean";

    const auto start = get_now();

    int degree = 50, n = 1000;
    AKNNG aknng(degree, distance);
    aknng.build(data_path, n);

    string save_path = base_dir + "index/aknng/sift/data1m-d50/";
    aknng.save(save_path);

    const auto end = get_now();
    cout << get_duration(start, end) / 1000000 / 60 << " [min]" << endl;
}
