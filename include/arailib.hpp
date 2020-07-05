#ifndef ARAILIB_ARAILIB_HPP
#define ARAILIB_ARAILIB_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <cmath>
#include <fstream>
#include <sstream>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <omp.h>
#include <json.hpp>

using namespace std;
using namespace nlohmann;

namespace arailib {
    template<typename UnaryOperation, typename Iterable>
    Iterable fmap(UnaryOperation op, const Iterable &v) {
        Iterable result;
        std::transform(v.begin(), v.end(), std::back_inserter(result), op);
        return result;
    }

    template<typename Predicate, typename Iterable>
    Iterable filter(Predicate pred, const Iterable &v) {
        Iterable result;
        std::copy_if(v.begin(), v.end(), std::back_inserter(result), pred);
        return result;
    }

    template <typename T = double>
    struct Data {
        size_t id;
        std::vector<T> x;

        Data() : id(0), x({0}) {}

        Data(size_t i, std::vector<T> v) {
            id = i;
            std::copy(v.begin(), v.end(), std::back_inserter(x));
        }

        Data(std::vector<T> v) {
            id = 0;
            std::copy(v.begin(), v.end(), std::back_inserter(x));
        }

        auto& operator [] (size_t i) { return x[i]; }
        const auto& operator [] (size_t i) const { return x[i]; }

        bool operator==(const Data &o) const {
            if (id == o.id) return true;
            return false;
        }

        bool operator!=(const Data &o) const {
            if (id != o.id) return true;
            return false;
        }

        size_t size() const { return x.size(); }
        auto begin() const { return x.begin(); }
        auto end() const { return x.end(); }

        void show() const {
            std::cout << id << ": ";
            for (const auto &xi : x) {
                std::cout << xi << ' ';
            }
            std::cout << std::endl;
        }
    };

    template <typename T = double>
    using Dataset = vector<Data<T>>;

    template <typename T = double>
    using Series = vector<Data<T>>;

    template <typename T = double>
    using RefSeries = vector<reference_wrapper<const Data<T>>>;

    template <typename T = double>
    using SeriesList = vector<vector<Data<T>>>;

    template <typename T = double>
    using DistanceFunction = function<double(Data<T>, Data<T>)>;

    template <typename T = double>
    auto euclidean_distance(const Data<T>& p1, const Data<T>& p2) {
        float result = 0;
        for (size_t i = 0; i < p1.size(); i++) {
            result += std::pow(p1[i] - p2[i], 2);
        }
        result = std::sqrt(result);
        return result;
    }

    template <typename T = double>
    auto manhattan_distance(const Data<T>& p1, const Data<T>& p2) {
        float result = 0;
        for (size_t i = 0; i < p1.size(); i++) {
            result += std::abs(p1[i] - p2[i]);
        }
        return result;
    }

    template <typename T = double>
    auto l2_norm(const Data<T>& p) {
        float result = 0;
        for (size_t i = 0; i < p.size(); i++) {
            result += std::pow(p[i], 2);
        }
        result = std::sqrt(result);
        return result;
    }

    template <typename T = double>
    auto clip(const T val, const T min_val, const T max_val) {
        return max(min(val, max_val), min_val);
    }

    template <typename T = double>
    auto cosine_similarity(const Data<T>& p1, const Data<T>& p2) {
        float val = inner_product(p1.begin(), p1.end(), p2.begin(), 0.0)
            / (l2_norm(p1) * l2_norm(p2));
        return clip(val, static_cast<float>(-1), static_cast<float>(1));
    }

    constexpr float pi = static_cast<const float>(3.14159265358979323846264338);

    template <typename T = double>
    auto angular_distance(const Data<T>& p1, const Data<T>& p2) {
        return acos(cosine_similarity(p1, p2)) / pi;
    }

    auto select_distance(const string& distance) {
        if (distance == "euclidean") return euclidean_distance<double>;
        if (distance == "manhattan") return manhattan_distance<double>;
        if (distance == "angular")   return angular_distance<double>;
        else throw runtime_error("invalid distance");
    }

    template <typename T = double>
    vector<T> split(string &input, char delimiter = ',') {
        std::istringstream stream(input);
        std::string field;
        std::vector<T> result;

        while (std::getline(stream, field, delimiter)) {
            result.push_back(std::stod(field));
        }

        return result;
    }

    template <typename T = double>
    Dataset<T> read_csv(const std::string &path, const int& nrows = -1,
                        const bool &skip_header = false) {
        std::ifstream ifs(path);
        if (!ifs) throw runtime_error("Can't open file!");
        std::string line;

        Dataset<T> series;
        for (size_t i = 0; (i < nrows) && std::getline(ifs, line); ++i) {
            // if first line is the header then skip
            if (skip_header && (i == 0)) continue;
            std::vector<T> v = split<T>(line);
            series.push_back(Data<T>(i, v));
        }
        return series;
    }

    const int n_max_threads = omp_get_max_threads();

    template <typename T = double>
    Dataset<T> load_data(const string& path, int n = 0) {
        // file path
        if (path.rfind(".csv", path.size()) < path.size()) {
            auto series = Dataset<T>();
            ifstream ifs(path);
            if (!ifs) throw runtime_error("Can't open file!");
            string line;
            for (size_t i = 0; (i < n) && std::getline(ifs, line); ++i) {
                auto v = split(line);
                series.push_back(Data<T>(i, v));
            }
            return series;
        }

        // dir path
        auto series = Dataset<T>(n * 1000);
#pragma omp parallel for
        for (int i = 0; i < n; i++) {
            const string data_path = path + '/' + to_string(i) + ".csv";
            ifstream ifs(data_path);
            if (!ifs) throw runtime_error("Can't open file!");
            string line;
            while(getline(ifs, line)) {
                auto v = split(line);
                const auto id = static_cast<size_t>(v[0]);
                v.erase(v.begin());
                series[id] = Data<T>(id, v);
            }
        }
        return series;
    }

    template<typename T>
    void write_csv(const std::vector<T> &v, const std::string &path) {
        std::ofstream ofs(path);
        for (const auto &o : v) {
            std::string line;
            for (const auto &e : o) {
                line += std::to_string(e) + ',';
            }
            line.pop_back();
            line += '\n';
            ofs << line;
        }
    }

    json read_config(const string& config_path = "./config.json") {
        json config;
        ifstream ifs(config_path);
        if (ifs.fail()) throw runtime_error("Can't open file!");
        ifs >> config;
        return config;
    }

    auto get_now() { return chrono::system_clock::now(); }

    auto get_duration(chrono::system_clock::time_point start,
                      chrono::system_clock::time_point end) {
        return chrono::duration_cast<chrono::microseconds>(end - start).count();
    }

    bool is_csv(const string& path) {
        return (path.rfind(".csv", path.size()) < path.size());
    }

    template <typename T>
    auto scan_knn_search(const Data<T>& query, int k, const Dataset<T> dataset,
                         string distance = "euclidean") {
        const auto df = select_distance(distance);
        map<float, reference_wrapper<const Data<T>>> result_map;
        for (const auto& data : dataset) {
            const auto dist = euclidean_distance(query, data);
            result_map.emplace(dist, data);
            if (result_map.size() > k) result_map.erase(--result_map.cend());
        }

        RefSeries<T> result;
        for (const auto& result_pair : result_map) {
            result.emplace_back(result_pair.second);
        }

        return result;
    }
}

#endif //ARAILIB_ARAILIB_HPP