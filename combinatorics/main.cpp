#include <iostream>
#include <unordered_set>

#include "graph/graph.h"
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
#ifdef GTESTS
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif

    freopen("tests/1.txt", "r", stdin);

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

    auto res = graph.isDoublyConnected('b', 'r', 0);
    if (res.first) {
        for (size_t k = 0, mk = idToVert.size(); k < mk; k++) {
            std::cout << idToVert[k] << ' ' << res.second.atVertData(k) << ' ';
        }
    }
    else
    {
        std::cout << "graph doesn't doubly connected\n";
    }
    std::cout << std::endl;

    return 0;
}