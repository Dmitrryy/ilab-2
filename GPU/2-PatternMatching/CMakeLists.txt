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




set(FB_TARGET pm_pre)
add_custom_target(${FB_TARGET}
        #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )


# set the project name
project(PatternMatching)

find_package(OpenCL REQUIRED)

SET(MY_COMPILE_FLAGS "-lOpenCL")

# add the executable
add_executable(${PROJECT_NAME} main.cpp native_cpu/native_cpu.cpp)

if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${MY_COMPILE_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})

add_dependencies(${PROJECT_NAME} ${FB_TARGET})
