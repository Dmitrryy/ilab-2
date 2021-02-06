cmake_minimum_required(VERSION 3.10)


set(FB_TARGET pre_build_graph)
add_custom_target(${FB_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )


# set the project name
project(Graph)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# add the executable
add_executable(${PROJECT_NAME} main.cpp)


if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

if(GTESTS)
    add_compile_definitions(${PROJECT_NAME} PRIVATE GTESTS)
    target_sources(${PROJECT_NAME} PRIVATE gtests.cpp)
endif()


target_include_directories(${PROJECT_NAME} PRIVATE ../include ${GTEST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} Threads::Threads)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})
