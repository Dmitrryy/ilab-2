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


add_executable(${PROJECT_NAME} app_main.cpp Application/Driver.cpp Application/vkCore.cpp
        Application/App.cpp Application/Helpa/helpa.cpp Application/Entity.cpp gen_tests.hpp #[[Engine/engine.cpp Engine/engine.hpp Engine/mesh.cpp Engine/mesh.hpp Engine/types.hpp Engine/initializer.cpp Engine/initializer.hpp]] Application/PipelineBuilder.cpp Application/PipelineBuilder.hpp Application/Mesh.cpp Application/Mesh.hpp)


##############################
# Build mode dependent actions
if( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()
##############################


#add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)


# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")


# To connect other libraries (only OtherLibs)
target_include_directories(${PROJECT_NAME} PRIVATE ../../)
target_include_directories(${PROJECT_NAME} PRIVATE ../path)

# Link dependencies
target_link_libraries(${PROJECT_NAME} LAL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})


# see above
add_dependencies(${PROJECT_NAME} ${FB_TARGET})
##=======================================================================================
##=======================================================================================
#