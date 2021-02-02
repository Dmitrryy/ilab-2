cmake_minimum_required(VERSION 3.10)


set(FB_TARGET pre_build)
add_custom_target(${FB_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.cc ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.cc
        COMMAND bison parser.y
        COMMAND flex parser.l
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )

# set the project name
project(Current)

# add the executable
add_executable(${PROJECT_NAME} main.cpp ../Libs/matrix/Matrix_other.cpp lex.yy.cc parser.tab.cc)

if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_sources(${PROJECT_NAME} PRIVATE tests/gen_tests.cpp)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})

