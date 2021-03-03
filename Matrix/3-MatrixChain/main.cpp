/*************************************************************************
 *
 * main.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/


//
///======================================================================================
/// A program for calculating the optimal order of matrix multiplication.
///
/// There are two targets for assembly:
/// 1. tMatrixChain        - The output is only the optimal order of operations.
/// 2. tMatrixChainMeasure - At the output, measurements of time and the number of
///                         operations for the usual and optimal orders.
///
/// assembly instruction(in cur directory):
/// $: mkdir build
/// $: cd build
/// $: cmake ..
/// $: make
///======================================================================================
///======================================================================================
//


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
    std::ifstream in("tests/6.txt");
#else
    auto& in = std::cin;
    //std::ifstream in("tests/6.txt");
    //freopen("tests/my.txt", "w", stdout);

#endif


    ezg::MatrixChain< int > chain;

    while(in)
    {
        size_t left = 0, right = 0;
        in >> left >> right;

        if (!in) { break; }

        chain.addMatrix(matrix::Matrix< int >::random(left, right, -5, 5));
    }


#ifndef MEASUREMENTS

    chain.setOptimalOrder();
    auto Order = chain.getCurrentOrder();

    for (auto&& id : Order) {
        std::cout << id << ' ';
    }

#else // go tMatrixChainMeasure target.
    const size_t defOperations = chain.getNumReqOperations();

    Timer t;

    t.reset();
    //by default order is not optimal.
    auto res1 = chain.multiplication();
    const double defTimeMul = t.elapsed();

    std::cout << "default order takes:\n" << defTimeMul << " sec(mul)" << std::endl;
    std::cout << "operations: " << defOperations << std::endl;

    auto Order = chain.getCurrentOrder();
    std::cout << "order: ";
    for (auto&& id : Order) {
        std::cout << id << ' ';
    }
    std::cout << std::endl << std::endl;



    t.reset();
    chain.setOptimalOrder();
    Order = chain.getCurrentOrder();

    const double optTimeSetOrder = t.elapsed();
    t.reset();
    auto res2 = chain.multiplication();
    const double optTimeMul = t.elapsed();

    const size_t optOperations = chain.getNumReqOperations();




    std::cout << "optimal order takes:\n" << optTimeMul << " sec(mul) + "
                << optTimeSetOrder << " sec(setOrder) = " << optTimeMul + optTimeSetOrder << " sec" << std::endl;
    std::cout << "operations: " << optOperations << std::endl;

    std::cout << "order: ";
    for (auto&& id : Order) {
        std::cout << id << ' ';
    }
    std::cout << std::endl << std::endl;

#endif // MEASUREMENTS

    return 0;
}