cmake_minimum_required(VERSION 3.10)
project(aknng)

set(CMAKE_CXX_STANDARD 17)

add_executable(aknng main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O3 -march=native")

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(test)