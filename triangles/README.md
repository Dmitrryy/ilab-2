# Triangles
Second project to the Ilab course.

## Levels

#### LVL1 (done): 
the input is the number N followed by N of triangles (in 3D). 
The output is the numbers of intersecting triangles. 
Sell not for N ^ 2.

##### Algorithm
- Two triangles. \
LALib is a small library to help you with linear algebra. \
To view the algorithm of intersection of two triangles 
see source file /LAL/math/LALmath.cpp (~830 line).
    
- [Octree](https://en.wikipedia.org/wiki/Octree) \
Holder file: /Octree/Octree.h \
![](resource/otherImages/Octree.png "example") 

#### LVL2 (done):
Render your triangles using the [Vulkan API](https://www.khronos.org/vulkan/).
A 3D scene should consist of:
* Fixed volume where all triangles are located
* All triangles drawn in blue
* Triangles intersecting with others, drawn in red
* There should be at least trivial lighting
* And it should be possible to control the camera, flying around the scene, 
zooming in and out of the camera (with a mouse or keyboard)

#### LVL3 (in dev):
The rotation of the triangles

At this level, you are invited to expand your shell and your solution to the triangle intersection problem.

The idea is as follows. Now, in each input file, in addition to each triangle itself, additional parameters are set: (1) two points through which the axis of its rotation passes, and (2) the speed of rotation

For example:

0.0 -1.0 0.0 0.0 1.0 0.0 1.0 0.0 0.0 \
0.0 0.0 0.0 1.0 0.0 0.0 \
10.0

This is a triangle, initially lying in the XY plane, which rotates around the 
X-axis at a speed of 10 degrees per second

All rotation is clockwise, so after 9 seconds, this triangle will take the form:

0.0 0.0 -1.0 0.0 0.0 1.0 1.0 0.0 0.0

And after 18 seconds, it will align with itself, changing the direction

0.0 1.0 0.0 0.0 -1.0 0.0 1.0 0.0 0.0

The input file must have the following format:

number of triangles \
time (integer number of seconds) \
triangle \
rotation axis rotation \
speed \
triangle rotation \
axis rotation \
speed \
....

Your task is to visualize a system of rotating triangles using the Vulkan API. 
The triangles should turn red when they intersect with someone and turn green 
again when they leave the intersection.

Try for example the following input data:

2 \
18 \
0.0 -1.0 0.0 0.0 1.0 0.0 1.0 0.0 0.0 \
0.0 0.0 0.0 1.0 0.0 0.0 \
10.0 \
0.0 -1.0 0.5 0.0 1.0 0.5 1.0 0.0 0.5 \
0.0 0.0 0.5 1.0 0.0 0.5 \
-10.0 

## License
GNU General Public License v3.0

