cmake_minimum_required(VERSION 3.10)


set(FB_TARGET bs_pre_vulkan2)
add_custom_target(${FB_TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resource ${CMAKE_CURRENT_BINARY_DIR}/resource

        COMMAND glslc ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/vert.spv
        COMMAND glslc ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/shader.frag -o ${CMAKE_CURRENT_BINARY_DIR}/resource/shaders/frag.spv
        )


#second level of triangles
#see README for details
project(VulkanApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} app_main.cpp Application/Driver.cpp Application/vkCore.cpp
        Application/App.cpp Application/Helpa/helpa.cpp Application/Entity.cpp gen_tests.hpp)

if(NOT LAL_FOUND)
    add_subdirectory(../LAL ../LAL/build)
endif()

add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)


# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")

target_include_directories(${PROJECT_NAME} PRIVATE ../../)

# Link dependencies
target_link_libraries(${PROJECT_NAME} LAL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})


add_dependencies(${PROJECT_NAME} ${FB_TARGET})