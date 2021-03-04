
#this is a cmake for assembling projects from the ilab_2 course
#by Dmitrryy

cmake_minimum_required(VERSION 3.10)

# set the project name
project(ilab)


add_subdirectory(cache)


add_subdirectory(triangles)


add_subdirectory(Matrix)


add_subdirectory(ParaCL)


add_subdirectory(OpenCL)


add_subdirectory(combinatorics)