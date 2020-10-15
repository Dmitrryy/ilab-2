cmake_minimum_required(VERSION 3.10)

# set the project name
project(Triangles)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Triangles main.cpp other.cpp tests/test_gen.cpp LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp LAL/math/LALmath.cpp)