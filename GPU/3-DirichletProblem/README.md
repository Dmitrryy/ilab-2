
# LVL3 Dirichlet problem

## task description

This level is interesting because formally there is a simple solution through 
multiplication of matrices, but in reality the matrices are gigantic and very 
sparse, so they have to be packed and the problem immediately becomes interesting

Theory: [1]

Formal statement. At the input: the grid step, the square area (upper corner, lower corner)
and the values of the desired function at the border of this area (clockwise from the 
upper left corner: right, down, left, up)

0.01 0.0 1.0 1.0 0.0 1.0 1.01 1.02 .... (here, in steps of 0.01 squared from (0, 1) to 
(1, 0), I set the edge values with the function f (x, y) = x + y)

Your task is to solve the internal Dirichlet problem for the Laplace equation

The output is a grid approximation of the function in this area in a given step line 
by line from top to bottom and each line from left to right

## Sources
- [1] https://developer.nvidia.com/gpugems/gpugems2/part-vi-simulation-and-numerical-algorithms/chapter-44-gpu-framework-solving