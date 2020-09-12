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
        , m_dist{_min, _max}
        {}

    unsigned operator()() {
        return get();
    }

    unsigned get() { return m_dist(m_gen); }

    std::vector<unsigned> range(size_t _count) 
    {
        std::vector<unsigned> result(_count);

        for (size_t i = 0u; i != _count; i++) {
            result[i] = get();
        }

        return result;
    }

private:

    std::random_device m_rd; 
    std::mt19937 m_gen; 

    std::uniform_int_distribution<> m_dist;
};