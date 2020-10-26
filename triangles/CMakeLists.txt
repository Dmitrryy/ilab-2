cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/triangles)

###
project(vkShell)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} mainDraw.cpp)

target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)

# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")


# link other .h in /path
target_include_directories(${PROJECT_NAME} PRIVATE path)

# Link dependencies
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:${PROJECT_NAME}>/resource)

###
#
#Triangles LVL1
#
###
project(TrianglesLVL1)

add_executable(${PROJECT_NAME}
        main.cpp other.cpp tests/test_gen.cpp
        LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp
        LAL/math/LALmath.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE LAL/include)
target_include_directories(${PROJECT_NAME} PRIVATE Octree)
target_include_directories(${PROJECT_NAME} PRIVATE tests)