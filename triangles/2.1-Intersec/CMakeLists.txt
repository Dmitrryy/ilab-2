cmake_minimum_required(VERSION 3.10)

#first level of triangles
#see README for details
project(Intersection)

add_executable(${PROJECT_NAME} main.cpp other.cpp tests/test_gen.cpp)

if(NOT LAL_FOUND)
    add_subdirectory(../LAL ../LAL/build)
endif()

target_link_libraries(${PROJECT_NAME} LAL)