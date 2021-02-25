/*************************************************************************************************
 *
 *   gen_tests.cpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/
#pragma once

#include <iostream>

#include <OtherLibs/random.h>


namespace ezg
{
    template< typename T >
    void gen_tr3_test_rand(std::basic_ostream< T >& out, size_t numTriangles, size_t time)
    {
        out << numTriangles * numTriangles << '\n' << time << '\n';

        ezg::Random rand(-20, 20);

        for(size_t i = 1; i < numTriangles; ++i)
        {
            for (size_t k = 1; k < numTriangles; ++k)
            {
                out << k * 8 - 5 << ' ' << i * 8 << ' ' << 0 << ' ';
                out << (k + 1) * 8 + 5 << ' ' << i * 8 << ' ' << 0 << ' ';
                out << k * 8 + 4 << ' ' << (i + 1) * 8 - 1 << ' ' << 0 << '\n';

                out << k * 8 + 4 << ' ' << i * 8 << ' ' << 0 << ' ';
                out << k * 8 + 4 << ' ' << (i + 1) * 8 << ' ' << 0 << '\n';

                out << 30 << std::endl;
            }
        }
    }

}//namespace ezg