cmake_minimum_required(VERSION 2.8.9)
project(shrink)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Werror -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(SOURCES
 Shrink.cxx
 TestShrink.cxx
)

add_executable(test_shrink ${SOURCES})
target_link_libraries(test_shrink ${OpenCV_LIBS})
