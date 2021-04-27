cmake_minimum_required(VERSION 3.10)

#first level of triangles
#see README for details
project(Intersection)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} main.cpp other.cpp tests/test_gen.cpp tests/LAL_test.cpp)

if(NOT LAL_FOUND)
    add_subdirectory(../path/LAL ${CMAKE_CURRENT_BINARY_DIR}/LALBuild)
endif()

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} Threads::Threads)

target_link_libraries(${PROJECT_NAME} LAL)

#[[
enable_testing()
add_test(NAME    Intersec
        COMMAND ${PROJECT_NAME})]]
