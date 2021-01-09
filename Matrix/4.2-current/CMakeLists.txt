cmake_minimum_required(VERSION 3.10)

# set the project name
project(Current)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.cpp ../Libs/matrix/Matrix_other.cpp lex.yy.cc parser.tab.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD

        COMMAND bison
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD

        COMMAND flex
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.l
)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD

        COMMAND cp
        ${CMAKE_CURRENT_SOURCE_DIR}/test.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>/test.txt
)