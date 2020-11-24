# Matrix


## Levels

#### LVL1 (done): 
Calculate the determinant of the matrix 
(the complexity of the algorithm is not a factorial).

##### Algorithm:
Using the [Gauss algorithm](https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D0%93%D0%B0%D1%83%D1%81%D1%81%D0%B0), 
we reduce the matrix to a triangular one and calculate its trace.

#### LVL2 (in dev): 
The electrical circuit is set in the following format: \
p1 -- p2, r; eV

where p1, p2 -- nodes, r -- resistance, e-EMF of the source

For example:

1 -- 2, 4.0;         \
1 -- 3, 10.0;        \
1 -- 4, 2.0; -12.0   \
2 -- 3, 60.0;        \
2 -- 4, 22.0;        \
3 -- 4, 5.0;

We need to find the currents in the edges.

The sign for a current is its direction relative to an edge of the graph. \
Response format (not guaranteed to be correct):

1 -- 2: -0.442958 A  \
1 -- 3: -0.631499 A  \
1 -- 4: -1.07446 A   \
2 -- 3: -0.0757193 A \
2 -- 4: 0.367239 A   \
3 -- 4: -0.707219 A 

##### Algorithm:
