#include <iostream>
#include <fstream>
#include <unordered_set>

#include "test_gen.h"
#include "graph/graph.h"
#include <gtest/gtest.h>


int main(int argc, char* argv[])
{
#ifdef GTESTS
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif

#ifdef GEN_TESTS
/*    std::ofstream outTest("tests/6.txt");
    std::ofstream outAns ("tests/6a.txt");
    genDoublyConnected(outTest, outAns, 10000, 30000);*/

    freopen("tests/empty.txt", "r", stdin);
    //freopen("tests/ma.txt", "w", stdout);
#endif

    std::unordered_map< size_t, size_t > uniqVertices;
    std::vector< size_t > idToVert;
    ezg::Graph_t< char > graph;
    while(std::cin)
    {
        size_t v1 = 0, v2 = 0, width = 0;
        std::cin >> v1;
        std::cin.ignore(3);
        std::cin >> v2;
        std::cin.ignore(1);
        std::cin >> width;
        if (!std::cin) { break; }

        if (uniqVertices.count(v1) == 0) {
            uniqVertices[v1] = graph.addVertex(0);
            idToVert.push_back(v1);
        }
        if (uniqVertices.count(v2) == 0) {
            uniqVertices[v2] = graph.addVertex(0);
            idToVert.push_back(v2);
        }
        graph.addEdge(uniqVertices[v1], uniqVertices[v2]);
    }

    auto res = graph.isDoublyConnected();

    if(!res.first) {
        std::cout << "graph is not doubly connected!" << '\n' << "Cycle of odd length:" << '\n';
        for (const auto &v : res.second) {
            std::cout << idToVert[v] << ' ';
        }
        std::cout << std::endl;
    }
    else if(graph.vertSize() != 0)
    {
        graph.paint({ 'b', 'r' }, uniqVertices.at(1));
        for (size_t k = 0, mk = idToVert.size(); k < mk; k++) {
            std::cout << idToVert[k] << ' ' << graph.atVertData(k) << ' ';
        }
    }

    return 0;
}