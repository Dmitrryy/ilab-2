#include <iostream>
#include <fstream>

#include <matrix/chain/MatrixChain.h>
#include <other/timer.h>

#include <chrono>

using ezg::Timer;
using matrix::Matrix;

int main()
{

#ifndef NDEBUG
    std::ifstream in("tests/5.txt");
#else
    auto& in = std::cin;
#endif


    ezg::MatrixChain< int > chain;

    while(in)
    {
        size_t left = 0, right = 0;
        in >> left >> right;

        if (!in) { break; }

        chain.addMatrix(matrix::Matrix< int >::random(left, right, -5, 5));
    }

    const size_t defOperations = chain.getNumReqOperations();

    Timer t;

    t.reset();
    //by default order is not optimal.
    auto res1 = chain.multiplication();
    const double defTimeMul = t.elapsed();

    t.reset();
    chain.setOptimalOrder();
    const double optTimeSetOrder = t.elapsed();
    t.reset();
    auto res2 = chain.multiplication();
    const double optTimeMul = t.elapsed();

    const size_t optOperations = chain.getNumReqOperations();

    std::cout << "optimal order takes:\n" << optTimeMul << " sec(mul) + "
                << optTimeSetOrder << " sec(setOrder) = " << optTimeMul + optTimeSetOrder << " sec" << std::endl;
    std::cout << "operations: " << optOperations << std::endl << std::endl;

    std::cout << "default order takes:\n" << defTimeMul << " sec(mul)" << std::endl;
    std::cout << "operations: " << defOperations << std::endl << std::endl;

    assert(res1 == res2);

    return 0;
}