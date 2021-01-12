cmake_minimum_required(VERSION 3.10)

# set the project name
project(ParaC)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)


# add the executable
add_executable(${PROJECT_NAME} main.cpp Compiler/Node.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)