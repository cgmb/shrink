/* Authored by Cordell Bloor
   Copyright 2015
   MIT Licensed
*/
#ifndef TEST_SHRINK_H
#define TEST_SHRINK_H

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/operations.hpp>
#include "Shrink.h"

namespace test {

cv::Mat matFromVector(const std::vector<char>& in, size_t width) {
  if (in.empty()) {
    return cv::Mat(0, 0, CV_8U);
  }
  assert(in.size() % width == 0);
  return 255*cv::Mat(in.size()/width, width, CV_8U, (void*)in.data()).clone();
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

static size_t test_shrink() {
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
  };
  for (auto&& test : tests) {
    cv::Mat input = test.inputMat();
    cv::Mat result = cgmb::shrink_max(input);
    cv::Mat expected_result = test.expectedMat();
    cv::Mat diff = result != expected_result;
    if (cv::countNonZero(diff)) {
      std::cout << test.description
        << "\nactual:\n " << result
        << "\nexpected:\n " << expected_result
        << std::endl;
      ++failed;
    }
  }
  size_t passed = tests.size() - failed;
  std::cout << failed << " failed. " << passed << " passed." << std::endl;
  return failed;
}

}

int main() {
  return test::test_shrink();
}

#endif /* TEST_SHRINK_H */
