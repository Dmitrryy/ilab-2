/*************************************************************************
 *
 *   app_main.cpp
 *
 *   Created by dmitry
 *   01.11.2020
 *
 ***/

#include "gen_tests.hpp"

#include "Application/App.h"

#include <cmath>


int main()
{
#ifdef NDEBUG
    //std::istream& inPut = std::cin;
    std::ifstream inPut("tests/5.txt");

#else
    std::ofstream test("../../../triangles/3-VulkanApp/tests/5.txt");
    assert(test);
    ezg::gen_tr3_test_rand(test, 50, 100);
    return 0;

    std::ifstream inPut("tests/4.txt");
#endif

    size_t num = 0;
    int time = 0;
    inPut >> num >> time;

    ezg::AppLVL3 app;
    for (size_t i = 0; i < num; ++i) {
        glm::vec3 a, b, c, rot_dir_v1, rot_dir_v2;
        float speed = 0;
        inPut >> a.x >> a.y >> a.z
                >> b.x >> b.y >> b.z
                >> c.x >> c.y >> c.z;

        inPut >> rot_dir_v1.x >> rot_dir_v1.y >> rot_dir_v1.z
                >> rot_dir_v2.x >> rot_dir_v2.y >> rot_dir_v2.z;

        inPut >> speed;

        app.addTriangle(a - rot_dir_v1, b - rot_dir_v1, c - rot_dir_v1
                        , rot_dir_v1, rot_dir_v2 - rot_dir_v1, speed, time);
    }

    app.run();

    return 0;

}