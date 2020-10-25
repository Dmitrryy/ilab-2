#[[
cmake_minimum_required(VERSION 3.10)

# set the project name
project(TrianglesIntersec)

# specify the C++ standard
set(TrianglesIntersec CMAKE_CXX_STANDARD 17)
set(TrianglesIntersec CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(TrianglesIntersec other.cpp tests/test_gen.cpp LAL/Line.cpp LAL/LineSegment.cpp LAL/Plane.cpp LAL/Vector.cpp LAL/square.cpp LAL/triangle.cpp LAL/math/LALmath.cpp)
]]
cmake_minimum_required(VERSION 3.10.2)

project(vkShell)

set(vkShell CMAKE_CXX_STANDARD 17)
set(vkShell CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Vulkan REQUIRED)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)

target_link_libraries(vkShell Vulkan::Vulkan)

#[[add_subdirectory(path/glfw-3.3.2)
include_directories(path/glfw-3.3.2/include)
target_link_libraries(vkShell glfw ${GLFW_LIBRARIES})]]

# Поддиректории, содержащие нужные h-файлы
#[[include_directories(vkShell C:/VulkanSDK/1.2.154.1/Include)
include_directories(vkShell C:/Libs/glfw-3.3.2.bin.WIN64/include)
include_directories(vkShell C:/Libs/glm)]]
#  Библиотеки, которые нужны для сборки главной цели
#target_link_libraries(vkShell glfw3)
add_executable(vkShell mainDraw.cpp)