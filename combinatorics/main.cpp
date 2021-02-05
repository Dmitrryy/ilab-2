#include <iostream>

#include "graph/graph.h"


int main()
{
    ezg::Graph_t< size_t > graph;

    graph.addVertex(1);
    graph.addVertex(1);
    graph.addVertex(1);
    graph.addVertex(1);

    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(1, 2);

    graph.dumpTable(std::cout);
    std::cout << std::endl;

    graph.addVertex(1);


    graph.dumpTable(std::cout);

    return 0;
}