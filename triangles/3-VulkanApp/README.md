## LVL3 (in dev):
Render your triangles using the [Vulkan API](https://www.khronos.org/vulkan/).
A 3D scene should consist of:
* Fixed volume where all triangles are located
* All triangles drawn in blue
* Triangles intersecting with others, drawn in red
* There should be at least trivial lighting
* And it should be possible to control the camera, flying around the scene, 
zooming in and out of the camera (with a mouse or keyboard)

#### Solution description

###### Hotkey

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

#### Developer diary
* Day 1 \
Read the [official tutorial](https://vulkan-tutorial.com/). 
Honestly writing code as we read and trying to figure it out.

* Day 2 \
You understand that the tutorial is of little use (for me personally). 
You copy the text from the last part and experiment with it.
I catch an error with JSON but it seems to work.

* Day 3 \
Bump into a [book](https://www.litres.ru/grehem-sellers/vulkan-rukovodstvo-razrabotchika-oficialnoe-rukovo-27071853/).

* Day 5 \
Still reading a book. \
      + http://www.ogldev.org/www/tutorial50/tutorial50.html

* Day 6 \
Updating my IDE vs2019. That was a bad idea ... Now intellisence is sooooo slow and impossible to work.
(I'm waiting for 'softly' to release an update on win10 with blue screen support) \
Replacement: vs2019 -> CLion. \
I spend a lot of time trying to build a project through cmake, 
but I didn�t come to success 
(I caught a couple more errors (yes, I got banned from Google)). \
Replacement: Windows 10 -> Ubuntu. \
Another hour to install the components.
3rd hour of the night. \
The project is finally ready.

* 5 years later: \
sent for verification LVL2