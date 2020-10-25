cmake_minimum_required(VERSION 3.10)

# set the project name
project(ilab)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_subdirectory(cache)
add_subdirectory(triangles)
add_subdirectory(Matrix)