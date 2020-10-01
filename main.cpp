#include <arailib.hpp>
#include <aknng.hpp>

using namespace std;
using namespace arailib;
using namespace aknng;

int main() {
    string base_dir = "/home/arai/workspace/";
    string data_path = base_dir + "dataset/sift/data1m/";

    const auto start = get_now();

    int degree = 25, n = 10;
    AKNNG aknng(degree);
    aknng.build(data_path, n);

    string save_path = base_dir + "index/aknng/sift/data10k-d25/";
    aknng.save(save_path);

    const auto end = get_now();
    cout << get_duration(start, end) / 1000000 / 60 << " [min]" << endl;
}
