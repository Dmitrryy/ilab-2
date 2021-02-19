//===----------------------------------------------------------------------===//
///
///  Created by Dmitryyy (https://github.com/Dmitrryy)
///
///  Allow you to write off it.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <random>
#include <vector>
#include <matrix/Matrix.h>



namespace ezg {

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

}//namespace ezg