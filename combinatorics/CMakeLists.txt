cmake_minimum_required(VERSION 3.10)

# set the project name
project(Graph)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.cpp)


if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()