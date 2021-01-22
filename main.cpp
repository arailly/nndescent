#include <cpputil.hpp>
#include <nndescent.hpp>

using namespace std;
using namespace cpputil;
using namespace nndescent;

int main() {
    string base_dir = "/mnt/qnap/data";
    string data_path = base_dir + "/sift/sift_base.fvecs";

    const auto start = get_now();

    int n = 1000000, dim = 128, K = 50;
    AKNNG aknng(n, dim, K);
    aknng.build(data_path);

    string save_path = base_dir + "/sift/index/aknng/data1k-K20.csv";
    aknng.save(save_path);

    const auto end = get_now();
    cout << get_duration(start, end) / 1000000 / 60 << " [min]" << endl;
}
