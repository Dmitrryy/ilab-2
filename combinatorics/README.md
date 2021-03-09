# Combinatorics


## Levels

### LVL1 Graph (done):
You have a graph at the input in the usual representation

v1 - v2, edge_weight

For example:

1 -- 2, 4 \
2 -- 3, 5 \
3 -- 4, 6 \
4 -- 1, 1 

You need to read this graph into a more optimal representation (as in the lecture) and:

If the graph is not bipartite, output an error
If the graph is bipartite, color the vertices of the first segment blue 
(the color is an attribute of the vertex), 
the second segment red. Vertex color 1 is always blue.

Then display all vertices and the color of each

1 b 2 r 3 b 4 r

The edge weight is not used in this problem, 
it is needed so that you have something to parameterize your class for experiments



### LVL2 Spanning trees:

You can see that in the constructed representation of the graph, it is very easy 
to "remove" an edge, leaving it in place, but excluding it from the list of edges

To do this, just set:     \
next(prev(a)) = next (a)  \
prev(next (a)) = prev(a)  

The operation of "restoring" an edge that was removed in this way is also simple

In this problem, you are asked to implement the algorithm S from 7.2.1.6 TAOCP

The weight of a spanning tree is the total weight of its edges

Formal statement:

The input is a graph in the usual representation. \
The output is sorted in ascending order by the weight of all its spanning trees
