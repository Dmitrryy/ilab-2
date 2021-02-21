cmake_minimum_required(VERSION 3.10)


add_subdirectory(LAL)
set(LAL_FOUND TRUE)

add_subdirectory(1-Intersec)

add_subdirectory(3-VulkanApp)

#[[
#================================================================
#
#TrianglesGtest
#
#================================================================


project(TrianglesGtest)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} tests/LAL_test.cpp
        other.cpp tests/test_gen.cpp
        LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp
        LAL/math/LALmath.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../include
        ${GTEST_INCLUDE_DIRS}
        )


target_include_directories(${PROJECT_NAME} PRIVATE LAL/include)
target_include_directories(${PROJECT_NAME} PRIVATE Octree)
target_include_directories(${PROJECT_NAME} PRIVATE tests)

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} Threads::Threads)

enable_testing()

add_test(NAME    TrianglesGtest
        COMMAND ${PROJECT_NAME})
        ]]