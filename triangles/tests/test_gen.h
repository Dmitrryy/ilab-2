#pragma once

#include <functional>
#include <random>
#include <vector>

#include "../LAL/include/triangle.h"
#include "../LAL/include/square.h"

class Random
{
public:

    Random(unsigned _min = std::mt19937::min(), unsigned _max = std::mt19937::max())
        : m_rd()
        , m_gen(m_rd())
        , m_dist{ _min, _max }
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

std::vector<int> testGenTriangles(la::Square _area, size_t num, size_t rad = 5u);
la::Vector3f genPoint3f(la::Square _area);