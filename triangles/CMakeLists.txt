cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

###
project(vkShell)

find_package(Vulkan REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vkShell mainDraw.cpp)

target_link_libraries(vkShell ${X11_LIBRARIES})
add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)

# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
target_include_directories(vkShell PRIVATE "$ENV{VULKAN_SDK}/include")
target_link_directories(vkShell PRIVATE "$ENV{VULKAN_SDK}/lib")
target_link_directories(vkShell PRIVATE "$ENV{VULKAN_SDK}/etc/explicit_layer.d")


# link other .h
target_include_directories(vkShell PRIVATE path)

# Link dependencies
target_link_libraries(vkShell glfw)
target_link_libraries(vkShell Vulkan::Vulkan)




###
#
#Triangles LVL1
#
###
project(TrianglesLVL1)

add_executable(TrianglesLVL1 main.cpp other.cpp tests/test_gen.cpp LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp LAL/math/LALmath.cpp)

target_include_directories(TrianglesLVL1 PRIVATE LAL/include)
target_include_directories(TrianglesLVL1 PRIVATE Octree)
target_include_directories(TrianglesLVL1 PRIVATE tests)