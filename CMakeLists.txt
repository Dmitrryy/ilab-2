
#this is a cmake for assembling projects from the ilab_2 course
#by Dmitrryy

cmake_minimum_required(VERSION 3.10)

# set the project name
project(ilab)

#first homework
#status: final
add_subdirectory(cache)

#second homework
#status: LVL2 in dev
#
#LVL1: project's name for build is 'TrianglesLVL1'
add_subdirectory(triangles)

#...one more homework
#status: LVL2 in dev
#
#LVL1: project's name for build is 'Matrix'
add_subdirectory(Matrix)

#[[
#ParaCL
#status: LVL1 in dev
#
#LVL1: project's name for build is 'ParaCL']]
add_subdirectory(ParaCL)



add_subdirectory(OpenCL)