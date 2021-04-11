#[[
/****************************************************************************************
*
*   CmakeLists.txt
*
*   Created by dmitry
*   22.02.2021
*
***/
]]

cmake_minimum_required(VERSION 3.10)

#
## bs_pre_vulkan2
##=======================================================================================
## An additional target is created to execute preparatory commands before building
## the main project. In this case, this is copying the directories with tests and
## resources to the folder with the binary. And compilation of shaders.
set(FB_TARGET bs_pre_vulkan4)
add_custom_target(${FB_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resource ${CMAKE_CURRENT_BINARY_DIR}/resource

        COMMAND glslc ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/vert.spv
        COMMAND glslc ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/shader.frag -o ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/frag.spv
        )
##=======================================================================================
##=======================================================================================
#



#
## 2.4-Meshes
##=======================================================================================
project(2.4-Meshes)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# adding third party libraries
find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)
if(NOT LAL_FOUND)
    add_subdirectory(../LAL ${CMAKE_CURRENT_BINARY_DIR}/LALBuild)
endif()

#
## app
##---------------------------------------------------------------------------------------
set(EXECUTE_NAME app)
add_executable(${EXECUTE_NAME} app_main.cpp Engine/Engine.cpp Engine/vkCore.cpp
        Application/App.cpp Application/Entity.cpp Engine/PipelineBuilder.cpp Engine/Mesh.cpp
        ../GLFWWrap/Window.cpp ../GLFWWrap/GLFWWrap.cpp ../path/tinyxml2p.hpp)

target_sources(${EXECUTE_NAME} PRIVATE ../path/tinyxml2.cpp)

target_compile_features(${EXECUTE_NAME} PRIVATE cxx_std_17)

# Build mode dependent actions
if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${EXECUTE_NAME} PRIVATE DEBUG)
    target_compile_options(${EXECUTE_NAME} PRIVATE -Wall)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()


# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(${EXECUTE_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(${EXECUTE_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(${EXECUTE_NAME} PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")


# To connect other libraries (only OtherLibs)
target_include_directories(${EXECUTE_NAME} PRIVATE ../../)
target_include_directories(${EXECUTE_NAME} PRIVATE ../)
target_include_directories(${EXECUTE_NAME} PRIVATE .)
target_include_directories(${EXECUTE_NAME} PRIVATE ../path)

# Link dependencies
target_link_libraries(${EXECUTE_NAME} LAL)
target_link_libraries(${EXECUTE_NAME} glfw)
target_link_libraries(${EXECUTE_NAME} Vulkan::Vulkan)
target_link_libraries(${EXECUTE_NAME} ${X11_LIBRARIES})


# see above
add_dependencies(${EXECUTE_NAME} ${FB_TARGET})
##---------------------------------------------------------------------------------------
##---------------------------------------------------------------------------------------
#

#
## gen scene
##---------------------------------------------------------------------------------------
set(EXECUTE_NAME gen_scene)

add_executable(${EXECUTE_NAME} main_genScene.cpp)


if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${EXECUTE_NAME} PRIVATE DEBUG)
    target_compile_options(${EXECUTE_NAME} PRIVATE -Wall)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()


# To connect other libraries (only OtherLibs)
target_include_directories(${EXECUTE_NAME} PRIVATE ../../)
target_include_directories(${EXECUTE_NAME} PRIVATE ../)
target_include_directories(${EXECUTE_NAME} PRIVATE .)
target_include_directories(${EXECUTE_NAME} PRIVATE ../path)

target_sources(${EXECUTE_NAME} PRIVATE ../path/tinyxml2.cpp)
##---------------------------------------------------------------------------------------
##---------------------------------------------------------------------------------------


##=======================================================================================
##=======================================================================================
#