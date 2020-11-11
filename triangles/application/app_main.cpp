#include <vkCore.h>


int main()
{
    vks::Core vkCore("test");

    vks::WindowControl window;
    window.Init(800, 600);
    vkCore.Init(&window);

    return 0;
}
