cmake_minimum_required(VERSION 3.5)

add_executable(aknng_test test.cpp)

target_link_libraries(aknng_test gtest gtest_main)
include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O0")