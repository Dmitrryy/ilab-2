cmake_minimum_required(VERSION 3.13.4)

set(FB_TARGET fb_targ_paracl)
add_custom_target(${FB_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        )

# set the project name
project(ParaCL)


find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(LLVM REQUIRED)


flex_target(
        scanner
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/lex.l
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/lexer.cpp
        )

bison_target(
        grammar
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/gramar.y
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.cpp
        COMPILE_FLAGS "--defines=${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler.cpp.h"
)


add_flex_bison_dependency(scanner grammar)


if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")


# add the executable
add_executable(${PROJECT_NAME}
        main.cpp
        Compiler/Node.cpp
        ${BISON_grammar_OUTPUTS}
        ${FLEX_scanner_OUTPUTS}
        )

target_link_libraries(${PROJECT_NAME} ${FLEX_LIBRARIES} ${BISON_LIBRARIES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DUMPALL>
        )

add_dependencies(${PROJECT_NAME} ${FB_TARGET})