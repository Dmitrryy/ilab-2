cmake_minimum_required(VERSION 3.10)

#static build of the LAL library
add_library(LAL STATIC Line.cpp LineSegment.cpp Plane.cpp
        Vector.cpp square.cpp triangle.cpp
        math/LALmath.cpp
        )

set(LAL_FOUND TRUE)