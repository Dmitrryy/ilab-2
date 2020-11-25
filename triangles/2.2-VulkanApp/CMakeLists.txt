cmake_minimum_required(VERSION 3.10)

#first level of triangles
#see README for details
project(VulkanApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} app_main.cpp Application/App.cpp Application/vkCore.cpp Application/Helpa/helpa.cpp)

if(NOT LAL_FOUND)
    add_subdirectory(../LAL ../LAL/build)
endif()
add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)


# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")


# Link dependencies
target_link_libraries(${PROJECT_NAME} LAL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})

# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resource $<TARGET_FILE_DIR:${PROJECT_NAME}>/resource)
