/* Authored by Cordell Bloor
   Copyright 2015
   MIT Licensed
*/
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/operations.hpp>
#include "Shrink.h"

namespace test {

using std::chrono::steady_clock;
using std::chrono::duration_cast;

cv::Mat matFromVector(const std::vector<char>& in, size_t width) {
  if (in.empty()) {
    return cv::Mat(0, 0, CV_8U);
  }
  assert(in.size() % width == 0);
  return 255*cv::Mat(in.size()/width, width, CV_8U, (void*)in.data()).clone();
}

cv::Mat pretty(const cv::Mat& in) {
  return in / 255;
}

struct test_shrink_t {
  std::string description;
  std::vector<char> input;
  size_t input_width;
  std::vector<char> expected_result;
  size_t expected_result_width;

  cv::Mat inputMat() const {
    return matFromVector(input, input_width);
  }

  cv::Mat expectedMat() const {
    return matFromVector(expected_result, expected_result_width);
  }
};

static size_t test_shrink_max() {
  size_t failed = 0u;
  std::vector<test_shrink_t> tests = {
    { "null", {}, 0, {}, 0},
    { "1px", {1}, 1, {1}, 1},
    { "4px", {1,1,
              1,1}, 2,
             {0,0,
              0,1}, 2},
    { "9px-1", {0,0,0,
                0,1,0,
                0,0,0}, 3,
               {0,0,0,
                0,1,0,
                0,0,0}, 3},
    { "9px-9", {1,1,1,
                1,1,1,
                1,1,1}, 3,
               {0,0,0,
                0,1,0,
                0,0,0}, 3},
    { "25px-25", {1,1,1,1,1,
                  1,1,1,1,1,
                  1,1,1,1,1,
                  1,1,1,1,1,
                  1,1,1,1,1}, 5,
                 {0,0,0,0,0,
                  0,0,0,0,0,
                  0,0,1,0,0,
                  0,0,0,0,0,
                  0,0,0,0,0}, 5},
    { "25px-18", {1,1,0,1,1,
                  1,1,0,1,1,
                  0,0,0,0,0,
                  1,1,1,1,1,
                  1,1,1,1,1}, 5,
                 {0,0,0,0,0,
                  0,1,0,0,1,
                  0,0,0,0,0,
                  0,0,0,0,0,
                  0,0,1,0,0}, 5},
    { "25px-octave-bug",
      {0,0,0,1,0,
       1,1,1,1,0,
       0,0,1,1,0,
       0,0,1,1,0,
       0,0,0,1,0}, 5,
      {0,0,0,0,0,
       0,0,0,1,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0}, 5},
  };
  for (auto&& test : tests) {
    cv::Mat input = test.inputMat();
    cv::Mat result = cgmb::shrink_max(input);
    cv::Mat expected_result = test.expectedMat();
    cv::Mat diff = result != expected_result;
    if (cv::countNonZero(diff)) {
      std::cout << test.description
        << "\nactual:\n " << pretty(result)
        << "\nexpected:\n " << pretty(expected_result)
        << std::endl;
      ++failed;
    }
  }
  size_t passed = tests.size() - failed;
  std::cout << failed << " failed. " << passed << " passed." << std::endl;
  return failed;
}

void benchmark_shrink_max() {
  const int seed = 438729;
  std::mt19937 engine(seed);
  std::uniform_int_distribution<char> distribution(0, 1);
  auto rng = std::bind(distribution, engine);

  std::vector<char> v(1e6);
  std::generate(v.begin(), v.end(), rng);

  std::vector<test_shrink_t> tests = {
    { "benchmark 1", v, 1000, {}, 0},
  };

  for (auto&& test : tests) {
    cv::Mat input = test.inputMat();
    auto before = steady_clock::now();
    cv::Mat result = cgmb::shrink_max(input);
    auto after = steady_clock::now();
    unsigned long ns = duration_cast<std::chrono::nanoseconds>(after - before).count();
    std::cout << test.description << " (" << cv::countNonZero(result) << "): "
      << ns / 1e9 << "s" << std::endl;
  }
}

}

int main(int argc, char** argv) {
  if (argc == 2 && std::strcmp(argv[1], "--benchmark") == 0) {
    test::benchmark_shrink_max();
  }
  return test::test_shrink_max();
}
