## LVL3 (done):
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

### Solution description

#### Hotkey

| action              | key         |
|:-------------------:|:-----------:|
| exit                | esc         |
| release the coursor | TAB (keep)  |
| forward             | W           |
| backward            | S           |
| go to left          | A           |
| go to right         | D           |
| go up               | LShift      |
| go down             | Lctrl       |

#### Resource:
* [official tutorial](https://vulkan-tutorial.com/).
* book [Vulkan programming gide](https://www.litres.ru/grehem-sellers/vulkan-rukovodstvo-razrabotchika-oficialnoe-rukovo-27071853/).
* OGLdev [tutorial 50](http://www.ogldev.org/www/tutorial50/tutorial50.html).
* [Vulkan Guide](https://vkguide.dev/).