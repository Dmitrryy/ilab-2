/****************************************************************************************
 *
 * main.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/


//
///======================================================================================
/// The program checks the graph for bipartition.
/// The input is a graph in the form:
/// 1 - 2, 4
/// 2 - 3, 5
/// 3 - 4, 6
/// 4 - 1, 1
/// Output: 'vertex number' 'color' ... if the graph is bipartite
/// or a message that the graph is not bipartite with an example of an odd-length cycle.
/// Given the above, the output is:
/// 1 b 2 r 3 b 4 r
///
/// assembly instruction(in cur directory):
/// $: mkdir build
/// $: cd build
/// $: cmake ..
/// $: make
///
/// launch:
/// $: ./Graph < tests.1.txt
///======================================================================================
///======================================================================================
//


#include <iostream>
#include <fstream>
#include <unordered_set>

#include "test_gen.h"
#include "graph/graph.h"
#include <gtest/gtest.h>

//#define GEN_TESTS

int main(int argc, char* argv[])
{
#ifdef GTESTS
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif //GTESTS

#ifdef GEN_TESTS
    std::ofstream outTest("tests/6.txt");
    std::ofstream outAns ("tests/6a.txt");
    genDoublyConnected(outTest, outAns, 10000, 30000);
#endif //GEN_TESTS


#ifndef NDEBUG
    auto&& in  = std::ifstream("tests/1.txt");
    auto&& out = std::ofstream ("tests/my.txt");
#else
    auto&& in  = std::cin;
    auto&& out = std::cout;
#endif //NDEBUG


    std::unordered_map< size_t, size_t > uniqVertices;
    std::vector< size_t > idToVert;
    ezg::Graph_t< char > graph;
    while(in)
    {
        size_t v1 = 0, v2 = 0, width = 0;
        in >> v1;
        in.ignore(3);
        in >> v2;
        in.ignore(1);
        in >> width;
        if (!in) { break; }

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

    auto res = graph.isBipartite();

    if(!res.first) {
        out << "graph is not doubly connected!" << '\n' << "Cycle of odd length:" << '\n';
        for (const auto &v : res.second) {
            out << idToVert[v] << ' ';
        }
        out << std::endl;
    }
    else
    {
        graph.paint({ 'b', 'r' }, uniqVertices[1]);
        for (size_t k = 0, mk = idToVert.size(); k < mk; k++) {
            out << idToVert[k] << ' ' << graph.atVertData(k) << ' ';
        }
    }

    return 0;
}