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

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})


####################################################################

project(CurrenTest)


add_executable(${PROJECT_NAME} tests/main_test.cpp ../Libs/matrix/Matrix_other.cpp lex.yy.cc parser.tab.cc tests/gen_tests.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})


target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG)
#target_compile_options(${PROJECT_NAME} PRIVATE -O4)
