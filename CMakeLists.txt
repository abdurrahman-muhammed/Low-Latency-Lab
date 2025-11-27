cmake_minimum_required(VERSION 3.20)
project(low_latency_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(low_latency_lab
    src/main.cpp
    src/benchmarks/bench_spsc.cpp
)

target_include_directories(low_latency_lab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)