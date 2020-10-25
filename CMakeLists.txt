
#this is a cmake for assembling projects from the ilab_2 course
#by Dmitrryy

cmake_minimum_required(VERSION 3.10)

# set the project name
project(ilab)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#first homework
#status: final
add_subdirectory(cache)

#second homework
#status: LVL2 in dev
#
#LVL1: project's name for build is 'TrianglesLVL1'
add_subdirectory(triangles)

#...one more homework
#status: LVL1 in dev
#
#LVL1: project's name for build is 'Matrix'
add_subdirectory(Matrix)