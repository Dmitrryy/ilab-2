cmake_minimum_required(VERSION 3.10)

set(FB_TARGET fb_targ_paracl)
add_custom_target(${FB_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/parser/gramar.tab.cc ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex.yy.cc
        COMMAND bison gramar.y
        COMMAND flex lex.l
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )

# set the project name
project(ParaCL)

if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# add the executable
add_executable(${PROJECT_NAME} main.cpp Compiler/Node.cpp parser/lex.yy.cc parser/gramar.tab.cc)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_dependencies(${PROJECT_NAME} ${FB_TARGET})