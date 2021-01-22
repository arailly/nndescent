#include <arailib.hpp>
#include <nndescent.hpp>

using namespace std;
using namespace arailib;
using namespace nndescent;

int main() {
    string base_dir = "/home/arai/workspace/";
    string data_path = base_dir + "dataset/hepmass/mass1000/data7m/";
    string distance = "manhattan";

    const auto start = get_now();

    int degree = 40, n = 2000;
    AKNNG aknng(degree, distance);
    aknng.build(data_path, n);

    string save_path = base_dir + "index/nndescent/sift/data1m-d50/null/";
    aknng.save(save_path);

    const auto end = get_now();
    cout << get_duration(start, end) / 1000000 / 60 << " [min]" << endl;
}
