/* Authored by Cordell Bloor
   Copyright 2015
   MIT Licensed
*/
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <opencv2/core/core.hpp>
#include "Shrink.h"

namespace cgmb {

static const unsigned char M = 1;

static bool test_flag(const uint64_t* flags, unsigned char i) {
  unsigned char j = i / 64u;
  unsigned char k = i % 64u;
  uint64_t bitmask = uint64_t(1) << k;
  return flags[j] & bitmask;
}

static bool is_set(const cv::Mat& in, int row, int col) {
  return in.at<unsigned char>(row, col) != 0;
}

static unsigned char pattern_index(const cv::Mat& in,
  const int rows, const int cols, const int row, const int col) {
  assert(rows > 0);
  assert(cols > 0);
  assert(row >= 0);
  assert(col >= 0);
  assert(row < rows);
  assert(col < cols);

  unsigned char v = 0;
  if (row > 0) {
    if (col > 0) {
      if (is_set(in, row - 1, col - 1)) {
        v += (1 << 0);
      }
    }
    if (is_set(in, row - 1, col)) {
      v += (1 << 1);
    }
    if (col < cols - 1) {
      if (is_set(in, row - 1, col + 1)) {
        v += (1 << 2);
      }
    }
  }

  if (col > 0) {
    if (is_set(in, row, col - 1)) {
      v += (1 << 3);
    }
  }
  if (col < cols - 1) {
    if (is_set(in, row, col + 1)) {
      v += (1 << 4);
    }
  }

  if (row < rows - 1) {
    if (col > 0) {
      if (is_set(in, row + 1, col - 1)) {
        v += (1 << 5);
      }
    }
    if (is_set(in, row + 1, col)) {
      v += (1 << 6);
    }
    if (col < cols - 1) {
      if (is_set(in, row + 1, col + 1)) {
        v += (1 << 7);
      }
    }
  }

  return v;
}

static const uint64_t k_conditional[] = {
  0x8000cb0180d9cbde,
  0x00008b0100100001,
  0x8000000080dd0001,
  0x33d18b0100d10001,
};

static const uint64_t k_unconditional[] = {
  0xaefeb9e0cc19ca5a,
  0xabaebcb1bbfefca1,
  0xfefefefefca7fea8,
  0xfefefefef9a6fca3,
};

static bool check_conditional_match(unsigned char index) {
  return test_flag(k_conditional, index);
}

static bool check_unconditional_match(unsigned char index) {
  return test_flag(k_unconditional, index);
}

// input:  CV_8UC1
// output: CV_8UC1
cv::Mat shrink_max(cv::Mat in) {
  const int rows = in.size().height;
  const int cols = in.size().width;

  size_t erasure_count;
  do {
    erasure_count = 0;
    cv::Mat conditional = cv::Mat::zeros(in.size(), in.type());
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        if (in.at<unsigned char>(row, col) == 0) {
          continue; // this one can't be erased -- it's already empty!
        }

        unsigned char p = pattern_index(in, rows, cols, row, col);
        if (check_conditional_match(p)) {
          conditional.at<unsigned char>(row, col) = M;
        }
      }
    }

    cv::Mat out = conditional.clone();
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        // check for an unconditional match
        if(conditional.at<unsigned char>(row, col) == 0) {
          continue; // no need to bother if already 0
        }

        unsigned char p = pattern_index(conditional, rows, cols, row, col);
        if (check_unconditional_match(p)) {
          out.at<unsigned char>(row, col) = 0;
        }
      }
    }

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        if (out.at<unsigned char>(row, col) == M) {
          in.at<unsigned char>(row, col) = 0;
          ++erasure_count;
        }
      }
    }
  } while (erasure_count > 0);
  return in;
}

}
