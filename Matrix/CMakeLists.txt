cmake_minimum_required(VERSION 3.10)

# set the project name
project(Matrix)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Matrix)

# add the executable
add_executable(${PROJECT_NAME} main.cpp tests/gen_test.cpp matrix/Matrix_other.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE matrix)


#############################################


project(MatrixTest)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Matrix)

# add the executable
add_executable(${PROJECT_NAME} tests/gen_test.cpp matrix/Matrix_other.cpp tests/test_main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE matrix)
target_include_directories(${PROJECT_NAME} PRIVATE tests)