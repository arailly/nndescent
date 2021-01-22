## Overview
It is implementation of NNDescent: approximate KNN graph construction algorithm.

The main process is written in `include/nndescent.hpp` (header only), so you can use it easily.

Reference: Efficient K-Nearest Neighbor Graph Construction for Generic Similarity Measures (W. Dong et al., WWW2011)

## Example
```
#include <arailib.hpp> // utility library
#include <nndescent.hpp> // main library

using namespace std;
using namespace arailib;
using namespace nndescent;

int main() {
    string data_path = "path/to/data.csv";

    int degree = 15, n = 1000; // degree of AKNNG: approximate KNN graph and data size
    AKNNG nndescent(degree); // init AKNNG
    nndescent.build(data_path, n); // build AKNNG

    string save_path = "path/to/nndescent.csv";
    nndescent.save(save_path); // save nndescent as csv file
}
```

## Input Data Format
If you want to create AKNNG with this three vectors, `(0, 1), (2, 4), (3, 3)`, you must describe data.csv like following format:
```
0,1
2,4
3,3
```
