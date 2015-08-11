# shrink
A morphological shrink filter for binary images.

OpenCV does not come with an implementation of binary image shrink—a terrible
oversight which this library attempts to correct.

#### Interface

A single function is provided:

    cv::Mat cgmb::shrink_max(cv::Mat in);

It takes a `CV_8UC1` OpenCV matrix and returns a `CV_8UC1` matrix of the same
size. The value 255 is interpreted as black, and any lesser value is
interpreted as white.

Binary image shrink is applied to the given matrix until all black connected
components have been reduced to points, or rings in the case of components
with holes.

#### How to Install

There are just two files of importance:
`Shrink.h` and `Shrink.cxx`. They depend only on OpenCV and the
standard library. Include them in your project like any other source file.
Compiling at the maximum optimization level is recommended, as I've found it
significantly improves the speed of this function. That's important, because
this is a not a very fast implementation.

#### References

This implementation is based on the algorithm specified in
Digital Image Processing, 3rd & 4th Editions, by William K. Pratt.
