cmake_minimum_required(VERSION 3.10)

set(FB_TARGET fb_targ)
add_custom_target(${FB_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
        COMMAND bison gramar.y
        COMMAND flex lex.l
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )

# set the project name
project(ParaCL)

#add_definitions(-DDEBUG)

# add the executable
add_executable(${PROJECT_NAME} main.cpp Compiler/Node.cpp parser/lex.yy.cc parser/gramar.tab.cc)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})

# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests)