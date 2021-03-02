#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <chrono>

#include <gtest/gtest.h>

#include "../Octree/Octree.h"
#include "tests/test_gen.h"

void intersectionN_N    (std::vector< la::Triangle > data);
void intersectionOctree (std::vector< la::Triangle > data);
std::vector< la::Triangle > getData(std::istream& _source);

#define GTESTS_RUN

int main(int argc, char** argv)
{

#ifdef GTESTS_RUN
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif

    auto data = getData(std::cin);

    intersectionN_N(data);

    return 0;
}
