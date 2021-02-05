cmake_minimum_required(VERSION 3.10)


set(FB_TARGET bs_pre)
add_custom_target(${FB_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )


# set the project name
project(BitonicSort)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCL REQUIRED)

SET(MY_COMPILE_FLAGS "-lOpenCL")

# add the executable
add_executable(${PROJECT_NAME} main.cpp)

if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${MY_COMPILE_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})

add_dependencies(${PROJECT_NAME} ${FB_TARGET})