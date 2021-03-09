## LVL4 (in dev):
A [triangle mesh](https://en.wikipedia.org/wiki/Triangle_mesh) is a kind of data structure
consisting of triangles, each of which shares a common edge with its neighbor.


We will assume that for this level, all the grids completely cover a region of space
that is homeomorphic to the sphere (that is, without holes and handles). The simplest
such mesh is a tetrahedron. In this case, any triangle will have an outer and an inner side.

You can suggest the following transformation:
a point is selected inside any triangle.
Next, it is shifted along the normal by a random distance in the direction from the area
of space bounded by the mesh. Obviously, such a transformation increases the number of
triangles by two (three are obtained from one source) and preserves the "sphericity"invariant.

Thus, from a tetrahedron, you can "grow" the most bizarre and strange shapes by making a
sufficiently large number of such transformations and each time choosing a random face.

* The first sub-task is a data structure for storing meshes. Perhaps you need two structures:
  MeshBuilder, sharpened for growth, and Mesh, well suited for rendering.

* The second sub-task is rendering meshes on Vulkan: several closed random meshes should
  fly around the box, collide, shimmer, and be colored red, at your discretion.

No special tests are provided here, the picture itself will be a pretty good criterion.

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