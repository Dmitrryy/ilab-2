# Matrix


## Levels

### LVL1 (done): 
Calculate the determinant of the matrix 
(the complexity of the algorithm is not a factorial).

#### Algorithm:
Using the [Gauss algorithm](https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D0%93%D0%B0%D1%83%D1%81%D1%81%D0%B0), 
we reduce the matrix to a triangular one and calculate its trace.

### LVL2 (done): 
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
Response format:

1 -- 2: 0.442958 A  \
1 -- 3: 0.631499 A  \
1 -- 4: -1.07446 A  \
2 -- 3: 0.0757193 A \
2 -- 4: 0.367239 A  \
3 -- 4: 0.707219 A 


### LVL3 (done):
The third level is devoted to matrix multiplication. It turns out that for sufficiently long strings of matrices, the order of their multiplication, if chosen correctly, saves a lot of computer time.

Suppose you have a 10x30 matrix A, 30x5 B, and 5x60 C.

Computing their product as A (BC) would take (30 × 5 × 60) + (10 × 30 × 60) = 27000 multiplications. At the same time, their product as (AB) C will give the same result in 4500 operations.

Your task is to write a MatrixChain class that allows you to:

1) Add matrix to chain
2) Print the optimal order of multiplication of the current matrix chain
3) Perform multiplication in optimal order for performance tests
4) Show obvious superiority over a naive approach

The matrix stack can be dynamically maintained and be quite large - tests for chains of 10-20 or more matrices are encouraged.

The dynamic programming method should help in choosing the optimal chain


### LVL4 Lazy matrices:

Calculations with matrices often result in buffers (when copying and copying assignment) 
where they are not needed.

Matrix a, b;                                                  \
... (form a) ...                                              \
b = a; // (1) created a buffer b is equal in magnitude to a   \
C = b * d;                                                    \
e = c + b;                                                    \
....                                                          \
a *= 3; // (2) here we need a separate buffer for a and b -- the matrix dispersed

All the time between (1) and (2), we kept the buffer for b in memory for nothing.

Your task is to define lazy copying and assignment of matrices-so that immediately 
after copying only the check box is checked, but all the matrices continue to use 
the common buffer as much as possible, and only if a write attempt is made, a valid
copy is made and the own buffer is allocated to the one that is being tried to change. 
Let's say:

(there are separate matrices a, b, c here)                                    \
Matrix d = a; // d and a share a common buffer                                \
b = d; / / the old buffer b is released, b, d and a share the common          \
a += c; // now a has a new buffer, b and d share the common buffer that used to be buffer a, and so on.

You also need to immerse this class in the hierarchy: make a common IMatrix interface 
so that ordinary and lazy matrices can be stored in the same array (for example, for 
a chain of calculations) and work with them in a uniform way