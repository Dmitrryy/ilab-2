cmake_minimum_required(VERSION 3.10)

# set the project name
project(Current)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.cpp ../Libs/matrix/Matrix_other.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.cc ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

#add_definitions(-DDEBUG)


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.cc
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND bison
        parser.y
        )

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.cc
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND flex
        parser.l
        )

#[[add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND rm
        lex.yy.cc parser.tab.cc
        )]]


# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests)