/*************************************************************************************************
 *
 *   gen_test.h
 *
 *   Created by dmitry
 *   10.03.2021
 *
 ***/


#pragma once

#include <functional>
#include <random>
#include <vector>


class Random
{
public:

    Random(unsigned _min = std::mt19937::min(), unsigned _max = std::mt19937::max())
            : m_rd()
            , m_gen(m_rd())
            , m_dist{ static_cast<int>(_min), static_cast<int>(_max) }
    {}

    int operator()() {
        return get();
    }

    int get() { return m_dist(m_gen); }

private:

    std::random_device m_rd;
    std::mt19937 m_gen;

    std::uniform_int_distribution<> m_dist;
};


template< typename T, typename U >
static void gen_test(std::basic_ostream< T >& outTest, std::basic_ostream< U >& outAns, size_t size)
{
    std::vector< int > test;
    test.reserve(size);

    Random rand(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    outTest << size << std::endl;
    for (size_t k = 0; k < size; k++) {
        test.push_back(rand());
        outTest << test.back() << ' ';
    }

    std::sort(test.begin(), test.end());

    for (size_t k = 0; k < size; k++) {
        outAns << test[k] << ' ';
    }
}