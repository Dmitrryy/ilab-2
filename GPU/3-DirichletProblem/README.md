
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

  Code from the article for easy reading:
    > Example 44-1. The Conjugate Gradient Solver
    > ```c
    > void clCGSolver::solveInit() 
    > {    
    >   Matrix->matrixVectorOp(CL_SUB, X, B, R); 
    >   // R = A * x - b    
    >   R->multiply(-1);  
    >   // R = -R    
    >   R->clone(P);  
    >   // P = R    
    >   R->reduceAdd(R, Rho);  
    >   // rho = sum(R * R);    
    > }  
    > 
    > void clCGSolver::solveIteration() 
    > {    
    >   Matrix->matrixVectorOp(CL_NULL, P, NULL,Q);  
    >   // Q = Ap;    
    >   P->reduceAdd(Q, Temp);  
    >   // temp = sum(P * Q);    
    >    Rho->div(Temp, Alpha);  
    >   // alpha = rho/temp;      
    >   X->addVector(P, X, 1, Alpha);  
    >   // X = X + alpha * P    
    >   R->subtractVector(Q, R, 1, Alpha);  
    >   // R = R - alpha * Q    
    >   R->reduceAdd(R, NewRho);  
    >   // newrho = sum(R * R);    
    >   NewRho->divZ(Rho, Beta);  
    >   // beta = newrho/rho      
    >   R->addVector(P, P, 1, Beta);  
    >   // P = R + beta * P;    
    >   clFloat *temp; temp=NewRho;    
    >   NewRho=Rho; Rho=temp;  
    >   // swap rho and newrho pointers  
    > }  
    > 
    > void clCGSolver::solve(int maxI) 
    > {    
    >   solveInit();    
    >   for (int i = 0; i < maxI; i++) 
    >       solveIteration();  
    > }      
    > 
    > int clCGSolver::solve(float rhoTresh, int maxI) 
    > {    
    >   solveInit(); Rho->clone(NewRho);    
    >   for (int i = 0; i < maxI && NewRho.getData() > rhoTresh; i++)       
    >       solveIteration();    return i;  
    > } 
    > ```
    > 
    > The new values for y can be computed easily with our framework by applying one matrix-vector operation:
    > ````c
    > clMatrix->matrixVectorOp(CL_SUB, clCurrent, clLast, clNext);    
    > clLast->copyVector(clCurrent);        
    > // save for next iteration  
    > clCurrent->copyVector(clNext);        
    > // save for next iteration  
    > cluNext->unpack(clNext);              
    > // unpack for rendering    
    > renderHF(cluNext->m_pVectorTexture);  
    > // render as height field 
    > ````
    > 
    > The program to implicitly solve the wave equation now looks like this
    > ````c
    > cluRHS->computeRHS(cluLast, cluCurrent); 
    > // generate c(i, j, t)  
    > clRHS->repack(cluRHS);                   
    > // encode into RGBA    
    > iSteps = pCGSolver->solve(iMaxSteps);    
    > // solve using CG    
    > cluLast->copyVector(cluCurrent);         
    > // save for next iteration    
    > clNext->unpack(cluCurrent);              
    > // unpack for rendering  
    > renderHF(cluCurrent->m_pVectorTexture); 
    > ```` 