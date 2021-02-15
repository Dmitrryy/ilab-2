#include <iostream>
#include <fstream>

#include <matrix/chain/MatrixChain.h>

#include <chrono>


class Timer
{
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t> m_beg;

public:
    Timer() : m_beg(clock_t::now())
    {
    }

    void reset()
    {
        m_beg = clock_t::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};


int main()
{

#ifndef NDEBUG
    std::ifstream in("tests/3.txt");
#else
    auto& in = std::cin;
#endif



    ezg::MatrixChain< int > chain;

    while(in)
    {
        size_t left = 0, right = 0;
        in >> left >> right;

        if (!in) { break; }

        chain.add(matrix::Matrix< int >(left, right));
    }
    Timer t;

    t.reset();
    chain.optimalMultiplication();
    const double optTime = t.elapsed();

    t.reset();
    chain.defaultMultiplication();
    const double defTime = t.elapsed();


    auto optimalTree = chain.optimalOrderId();
    auto defaultTree = chain.defaultOrderId();

    std::cout << "optimal order take " << optTime << " sec" << std::endl;
    std::cout << "operations: " << optimalTree.at(optimalTree.getRootId()) << std::endl << std::endl;

    std::cout << "default order take " << defTime << " sec" << std::endl;
    std::cout << "operations: " << defaultTree.at(defaultTree.getRootId()) << std::endl << std::endl;

    return 0;
}