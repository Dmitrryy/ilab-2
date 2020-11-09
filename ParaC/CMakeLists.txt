cmake_minimum_required(VERSION 3.10)

# set the project name
project(ParaC)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ParaC)

# add the executable
add_executable(${PROJECT_NAME} main.cpp Compiler/ParaCL.cpp)