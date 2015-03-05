/* Authored by Cordell Bloor
   Copyright 2015
   MIT Licensed
*/
#include <assert.h>
#include <string.h>
#include <opencv2/core/core.hpp>
#include "Shrink.h"

namespace cgmb {

// the value which represents black in the binary image
static const unsigned char I = 255;

// Digital Image Processing, 3rd Edition, Table 14.3-1, pp.413,414
// Shrink, Thin, and Skeletonize Conditional Mark Patterns [M = 1 if hit]
static const unsigned char k_conditional_patterns[][9] = {
// S 1
{0,0,I,
 0,I,0,
 0,0,0},
{I,0,0,
 0,I,0,
 0,0,0},
{0,0,0,
 0,I,0,
 I,0,0},
{0,0,0,
 0,I,0,
 0,0,I},

// S 2
{0,0,0,
 0,I,I,
 0,0,0},
{0,I,0,
 0,I,0,
 0,0,0},
{0,0,0,
 I,I,0,
 0,0,0},
{0,0,0,
 0,I,0,
 0,I,0},

// S 3
{0,0,I,
 0,I,I,
 0,0,0},
{0,I,I,
 0,I,0,
 0,0,0},
{I,I,0,
 0,I,0,
 0,0,0},
{I,0,0,
 I,I,0,
 0,0,0},
{0,0,0,
 I,I,0,
 I,0,0},
{0,0,0,
 0,I,0,
 I,I,0},
{0,0,0,
 0,I,0,
 0,I,I},
{0,0,0,
 0,I,I,
 0,0,I},
/*
// TK 4
{0,I,0,
 0,I,I,
 0,0,0},
{0,I,0,
 I,I,0,
 0,0,0},
{0,0,0,
 I,I,0,
 0,I,0},
{0,0,0,
 0,I,I,
 0,I,0},
*/
// STK 4
{0,0,I,
 0,I,I,
 0,0,I},
{I,I,I,
 0,I,0,
 0,0,0},
{I,0,0,
 I,I,0,
 I,0,0},
{0,0,0,
 0,I,0,
 I,I,I},

// ST 5
{I,I,0,
 0,I,I,
 0,0,0},
{0,I,0,
 0,I,I,
 0,0,I},
{0,I,I,
 I,I,0,
 0,0,0},
{0,0,I,
 0,I,I,
 0,I,0},

// ST 5
{0,I,I,
 0,I,I,
 0,0,0},
{I,I,0,
 I,I,0,
 0,0,0},
{0,0,0,
 I,I,0,
 I,I,0},
{0,0,0,
 0,I,I,
 0,I,I},

// ST 6
{I,I,0,
 0,I,I,
 0,0,I},
{0,I,I,
 I,I,0,
 I,0,0},

// STK 6
{I,I,I,
 0,I,I,
 0,0,0},
{0,I,I,
 0,I,I,
 0,0,I},
{I,I,I,
 I,I,0,
 0,0,0},
{I,I,0,
 I,I,0,
 I,0,0},
{I,0,0,
 I,I,0,
 I,I,0},
{0,0,0,
 I,I,0,
 I,I,I},
{0,0,0,
 0,I,I,
 I,I,I},
{0,0,I,
 0,I,I,
 0,I,I},

// STK 7
{I,I,I,
 0,I,I,
 0,0,I},
{I,I,I,
 I,I,0,
 I,0,0},
{I,0,0,
 I,I,0,
 I,I,I},
{0,0,I,
 0,I,I,
 I,I,I},

// STK 8
{0,I,I,
 0,I,I,
 0,I,I},
{I,I,I,
 I,I,I,
 0,0,0},
{I,I,0,
 I,I,0,
 I,I,0},
{0,0,0,
 I,I,I,
 I,I,I},

// STK 9
{I,I,I,
 0,I,I,
 0,I,I},
{0,I,I,
 0,I,I,
 I,I,I},
{I,I,I,
 I,I,I,
 I,0,0},
{I,I,I,
 I,I,I,
 0,0,I},
{I,I,I,
 I,I,0,
 I,I,0},
{I,I,0,
 I,I,0,
 I,I,I},
{I,0,0,
 I,I,I,
 I,I,I},
{0,0,I,
 I,I,I,
 I,I,I},

// STK 10
{I,I,I,
 0,I,I,
 I,I,I},
{I,I,I,
 I,I,I,
 I,0,I},
{I,I,I,
 I,I,0,
 I,I,I},
{I,0,I,
 I,I,I,
 I,I,I},
/*
// K 11
{I,I,I,
 I,I,I,
 0,I,I},
{I,I,I,
 I,I,I,
 I,I,0},
{I,I,0,
 I,I,I,
 I,I,I},
{0,I,I,
 I,I,I,
 I,I,I},
*/
};

// Digital Image Processing, 3rd Edition, Table 14.3-I, p. 414
// Shrink, Thin, and Skeletonize Unconditional Mark Patterns
// P(M,M0,M1,M2,M3,M4,M5,M6,M7) = 1 if hit]

// A ∪ B ∪ C = 1
// A ∪ B = 1
// D = 0 ∪ 1
static const unsigned char M = 1;
static const unsigned char A = 2;
static const unsigned char B = 3;
static const unsigned char C = 4;
static const unsigned char D = 5;

static const unsigned char k_unconditional_patterns[][9] = {
// spur
{0,0,M,
 0,M,0,
 0,0,0},
{M,0,0,
 0,M,0,
 0,0,0},
{0,0,0,
 0,M,0,
 0,M,0},
{0,0,0,
 0,M,M,
 0,0,0},

// L Cluster
{0,0,M,
 0,M,M,
 0,0,0},
{0,M,M,
 0,M,0,
 0,0,0},
{M,M,0,
 0,M,0,
 0,0,0},
{M,0,0,
 M,M,0,
 0,0,0},
{0,0,0,
 M,M,0,
 M,0,0},
{0,0,0,
 0,M,0,
 M,M,0},
{0,0,0,
 0,M,0,
 0,M,M},
{0,0,0,
 0,M,M,
 0,0,M},

// offset
{0,M,M,
 M,M,0,
 0,0,0},
{M,M,0,
 0,M,M,
 0,0,0},
{0,M,0,
 0,M,M,
 0,0,M},
{0,0,M,
 0,M,M,
 0,M,0},

// spur corner
{0,A,M,
 0,M,B,
 M,0,0},
{M,B,0,
 A,M,0,
 0,0,M},
{0,0,M,
 A,M,0,
 M,B,0},
{M,0,0,
 0,M,B,
 0,A,M},

// corner clutter
{M,M,D,
 M,M,D,
 D,D,D},

// tee branch
{D,M,0,
 M,M,M,
 D,0,0},
{0,M,D,
 M,M,M,
 0,0,D},
{0,0,D,
 M,M,M,
 0,M,D},
{D,0,0,
 M,M,M,
 D,M,0},
{D,M,D,
 M,M,0,
 0,M,0},
{0,M,0,
 M,M,0,
 D,M,D},
{0,M,0,
 0,M,M,
 D,M,D},
{D,M,D,
 0,M,M,
 0,M,0},

// vee branch
{M,D,M,
 D,M,D,
 A,B,C},
{M,D,C,
 D,M,B,
 M,D,A},
{C,B,A,
 D,M,D,
 M,D,M},
{A,D,M,
 B,M,D,
 C,D,M},

// diagonal branch
{D,M,0,
 0,M,M,
 M,0,D},
{0,M,D,
 M,M,0,
 D,0,M},
{D,0,M,
 M,M,0,
 0,M,D},
{M,0,D,
 0,M,M,
 D,M,0},
};

static bool match(const unsigned char d[9], const unsigned char t[9]) {
  bool need_abc = false;
  bool a = false;
  bool b = false;
  bool c = false;
  for (size_t i = 0; i < 9; ++i) {
    switch(t[i]) {
      case 0:
        if (d[i] == M) {
          return false;
        }
        break;
      case M:
        if (d[i] == 0) {
          return false;
        }
        break;
      case A:
        if (d[i] == M) {
          a = true;
        } else {
          need_abc = true;
        }
        break;
      case B:
        if (d[i] == M) {
          b = true;
        } else {
          need_abc = true;
        }
        break;
      case C:
        if (d[i] == M) {
          c = true;
        } else {
          need_abc = true;
        }
        break;
      case D:
        break;
      default:
        assert(false);
        break;
    }
  }
  return !need_abc || (a || b || c);
}

static void fill_pixel_stack(const cv::Mat& in,
  const int rows, const int cols, const int row, const int col,
  unsigned char stack[9]) {
  // gather the pixels to compare
  size_t i = 0;
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (row + dr < 0 || row + dr == rows ||
          col + dc < 0 || col + dc == cols) {
        stack[i] = 0;
      } else {
        stack[i] = in.at<unsigned char>(row + dr, col + dc);
      }
      ++i;
    }
  }
}

// input:  CV_8UC1
// output: CV_8UC1
cv::Mat shrink_max(cv::Mat in) {
  const int rows = in.size().height;
  const int cols = in.size().width;

  unsigned char stack[9];
  size_t erasure_count;
  do {
    erasure_count = 0;
    cv::Mat conditional = cv::Mat::zeros(in.size(), in.type());
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        if (in.at<unsigned char>(row, col) == 0) {
          continue; // this one can't be erased -- it's already empty!
        }

        fill_pixel_stack(in, rows, cols, row, col, stack);

        // check for a conditional match
        for (size_t i = 0; i < sizeof(k_conditional_patterns)/9; ++i) {
          int diff = memcmp(&k_conditional_patterns[i][0], stack, 9);
          if (diff == 0) {
            conditional.at<unsigned char>(row, col) = M;
            break;
          }
        }
      }
    }

    cv::Mat out = conditional.clone();
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        fill_pixel_stack(conditional, rows, cols, row, col, stack);

        // check for an unconditional match
        if(conditional.at<unsigned char>(row, col) == 0) {
          continue; // no need to bother if already 0
        }
        for (size_t i = 0; i < sizeof(k_unconditional_patterns)/9; ++i) {
          if (match(stack, &k_unconditional_patterns[i][0])) {
            out.at<unsigned char>(row, col) = 0;
            break;
          }
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
