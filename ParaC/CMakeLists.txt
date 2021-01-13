cmake_minimum_required(VERSION 3.10)

# set the project name
project(ParaC)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

#add_definitions(-DDEBUG)


# add the executable
add_executable(${PROJECT_NAME} main.cpp Compiler/Node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex.yy.cc ${CMAKE_CURRENT_SOURCE_DIR}/parser/gramar.tab.cc
        )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

#[[add_custom_command(TARGET ${PROJECT_NAME}
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
        COMMAND rm
        lex.yy.cc gramar.tab.cc gramar.tab.hh stack.hh
        )]]

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser/gramar.tab.cc
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
        COMMAND bison -Wcex
        gramar.y
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex.yy.cc
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
        COMMAND flex
        lex.l
        )

# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests)