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
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/filter_GPU/kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl
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
add_executable(${PROJECT_NAME} main.cpp filter_GPU/PatternMatching.cpp)


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


set(MEASURE true)
if(MEASURE)
    set(MEASURE_TARGET_NAME measure)
    add_executable(${MEASURE_TARGET_NAME} main.cpp native_cpu/native_cpu.cpp filter_GPU/PatternMatching.cpp measure.cpp)

    add_dependencies(${MEASURE_TARGET_NAME} ${FB_TARGET})

    target_compile_definitions(${MEASURE_TARGET_NAME} PRIVATE MEASUREMENT)

    target_compile_options(${MEASURE_TARGET_NAME} PRIVATE ${MY_COMPILE_FLAGS})
    target_compile_features(${MEASURE_TARGET_NAME} PRIVATE cxx_std_17)

    find_package(OpenCL REQUIRED)
    target_include_directories(${MEASURE_TARGET_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
    target_link_libraries(${MEASURE_TARGET_NAME} ${OpenCL_LIBRARIES})
endif()


set(GEN_TESTS true)
if(GEN_TESTS)
    set(GEN_TESTS_TARGET_NAME gen_tests)
    add_executable(${GEN_TESTS_TARGET_NAME} main.cpp tests_src/gen_test.cpp)

    add_dependencies(${GEN_TESTS_TARGET_NAME} ${FB_TARGET})

    target_compile_definitions(${GEN_TESTS_TARGET_NAME} PRIVATE GEN_TESTS)

    target_compile_options(${GEN_TESTS_TARGET_NAME} PRIVATE ${MY_COMPILE_FLAGS})
    target_compile_features(${GEN_TESTS_TARGET_NAME} PRIVATE cxx_std_17)

    find_package(OpenCL REQUIRED)
    target_include_directories(${GEN_TESTS_TARGET_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
    target_link_libraries(${GEN_TESTS_TARGET_NAME} ${OpenCL_LIBRARIES})
endif()


# connecting the openCl library
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})
##=======================================================================================
##=======================================================================================
#
