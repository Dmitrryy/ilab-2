#[[
/****************************************************************************************
*
*   CmakeLists.txt
*
*   Created by dmitry
*   04.04.2021
*
***/
]]

cmake_minimum_required(VERSION 3.10)



#
## Dirichlet Problem
##=======================================================================================
project(DirichletProblem)


set(EXECUTE_NAME app)

# add the executable
add_executable(${EXECUTE_NAME} main.cpp)


# setting the 17th language standard
target_compile_options(${EXECUTE_NAME} PRIVATE ${MY_COMPILE_FLAGS})
target_compile_features(${EXECUTE_NAME} PRIVATE cxx_std_17)

target_include_directories(${EXECUTE_NAME} PRIVATE ../../Matrix/Libs)


# Build mode dependent actions
##-----------------------------------------------------------------------------
if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()
##-----------------------------------------------------------------------------
#

# connecting the openCl library
find_package(OpenCL REQUIRED)
target_include_directories(${EXECUTE_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${EXECUTE_NAME} ${OpenCL_LIBRARIES})
##=======================================================================================
##=======================================================================================
#
