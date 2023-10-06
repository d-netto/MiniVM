project(GCExperiment)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall)
add_compile_options(-O2)

include_directories(include)

add_subdirectory(src)