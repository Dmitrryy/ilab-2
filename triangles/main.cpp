#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <chrono>

//#define DEBUGG

#include "Octree/Octree.h"
#include "tests/test_gen.h"


void intersectionN_N    (std::vector< la::Triangle > data);
void intersectionOctree (std::vector< la::Triangle > data);
std::vector< la::Triangle > getData(std::istream& _source);

int main()
{
    //size_t num = 1000u;
    //double ax = -1000, ay = -1000, az = -1000, bx = 1000, by = 1000, bz = 1000;
    ////std::cin >> num >> ax >> ay >> az >> bx >> by >> bz;
    //std::ofstream out("tests/010rr.txt", std::ios::trunc);
    //if (out.is_open())
    //{
    //    out << num << '\n';
    //    auto res = testGenTriangles(la::Square({ ax, ay, az }, { bx, by, bz }), num, 10);
    //    for (const auto& i : res)
    //    {
    //        out << i << ' ';
    //    }
    //    out << std::endl;
    //    out.close();
    //}
    //else
    //{
    //    std::cerr << "cant open\n" << std::endl;
    //}
    //out.close();


    //std::ifstream in("tests/010rr.txt");
    auto data = getData(std::cin);
    //in.close();

    intersectionN_N(data);

    intersectionOctree(data);

    return 0;
}
