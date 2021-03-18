#[[
/****************************************************************************************
*
*   CmakeLists.txt
*
*   Created by dmitry
*   16.03.2021
*
***/
]]

cmake_minimum_required(VERSION 3.10)

#
## pm_pre
##=======================================================================================
## An additional target is created to execute preparatory commands before building
## the main project. In this case, this is copying the directories with tests.
set(FB_TARGET pm_pre)
add_custom_target(${FB_TARGET}
        #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )
##=======================================================================================
##=======================================================================================
#


#
## PatternMatching
##=======================================================================================
project(PatternMatching)


# add the executable
add_executable(${PROJECT_NAME} main.cpp native_cpu/native_cpu.cpp filter_GPU/PatternMatching.cpp filter_GPU/PatternMatching.hpp)


# before each assembly, commands from this dependency will be executed
add_dependencies(${PROJECT_NAME} ${FB_TARGET})


# setting the 17th language standard
target_compile_options(${PROJECT_NAME} PRIVATE ${MY_COMPILE_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


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
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})
##=======================================================================================
##=======================================================================================
#


add_subdirectory(tests_src)