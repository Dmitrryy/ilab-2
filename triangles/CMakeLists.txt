cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

###
project(vkShell)

find_package(Vulkan REQUIRED)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)

# Add vulkan dependencies using enviornment variable from LunarG SDK Setup
include_directories("$ENV{VULKAN_SDK}/include")
link_directories("$ENV{VULKAN_SDK}/lib")
link_directories("$ENV{VULKAN_SDK}/etc/explicit_layer.d")

# Add GLFW
find_package(glfw3 3.2 REQUIRED)

# link other .h
include_directories("path")

# Link dependencies
link_libraries(glfw)
link_libraries(Vulkan::Vulkan)

add_executable(vkShell mainDraw.cpp)


###
#
#Triangles LVL1
#
###
project(TrianglesLVL1)

add_executable(TrianglesLVL1 main.cpp other.cpp tests/test_gen.cpp LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp LAL/math/LALmath.cpp)
