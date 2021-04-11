# LVL 1 'intersections':

## task description

the input is the number N followed by N of triangles (in 3D).
The output is the numbers of intersecting triangles.
Sell not for N ^ 2.

## Solution description

- Two triangles. \
  LALib is a small library to help you with linear algebra. \
  To view the algorithm of intersection of two triangles
  see source file /LAL/math/LALmath.cpp (~830 line).

- [Octree](https://en.wikipedia.org/wiki/Octree) \
  Holder file: ../Octree/Octree.h \
  ![](../resource/otherImages/Octree.png "example")


# LVL2:

Render your triangles using the [Vulkan API](https://www.khronos.org/vulkan/).
A 3D scene should consist of:
* Fixed volume where all triangles are located
* All triangles drawn in blue
* Triangles intersecting with others, drawn in red
* There should be at least trivial lighting
* And it should be possible to control the camera, flying around the scene,
  zooming in and out of the camera (with a mouse or keyboard)