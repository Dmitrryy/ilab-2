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
#include "../LAL/include/LALmath.h"

#include <OtherLibs/random.h>


namespace ezg
{
    template< typename T >
    static void gen_tr3_test_net(std::basic_ostream< T >& out, size_t numTriangles_x, size_t numTriangles_y, size_t numTriangles_z, size_t time)
    {
        out << numTriangles_x * numTriangles_y * numTriangles_z << '\n' << time << '\n';

        ezg::Random rand(-20, 20);
        float offset_z = 20;
        float offset_x = -20;
        float offset_y = -20;
        for(size_t i = 1; i <= numTriangles_x; ++i)
        {
            for (size_t k = 1; k <= numTriangles_y; ++k)
            {
                for (size_t g = 1; g <= numTriangles_z; ++g)
                {
                    out << k * 8 - 5       + offset_x << ' ' << i * 8           + offset_y << ' ' << offset_z * (g - 1) << ' ';
                    out << (k + 1) * 8 + 5 + offset_x << ' ' << i * 8           + offset_y << ' ' << offset_z * (g - 1) << ' ';
                    out << k * 8 + 4       + offset_x << ' ' << (i + 1) * 8 - 1 + offset_y << ' ' << offset_z * (g - 1) << '\n';

                    out << k * 8 + 4       + offset_x << ' ' << i * 8           + offset_y << ' ' << offset_z * (g - 1) << ' ';
                    out << k * 8 + 4       + offset_x << ' ' << (i + 1) * 8     + offset_y << ' ' << offset_z * (g - 1) << '\n';

                    out << 30 << std::endl;
                }
            }
        }

/*        z = 20;
        for(size_t i = 1; i <= numTriangles; ++i)
        {
            for (size_t k = 1; k <= numTriangles; ++k)
            {
                out << k * 8 - 5       + offset_x << ' ' << i * 8           + offset_y << ' ' << z << ' ';
                out << (k + 1) * 8 + 5 + offset_x << ' ' << i * 8           + offset_y << ' ' << z << ' ';
                out << k * 8 + 4       + offset_x << ' ' << (i + 1) * 8 - 1 + offset_y << ' ' << z << '\n';

                out << k * 8 + 4       + offset_x << ' ' << i * 8           + offset_y << ' ' << z << ' ';
                out << k * 8 + 4       + offset_x << ' ' << (i + 1) * 8     + offset_y << ' ' << z << '\n';

                out << -30 << std::endl;
            }
        }*/
    }


    static la::Vector3f genPoint3f(la::Rectangle3 _area)
    {
        ezg::Random rx(_area.getA().x, _area.getB().x);
        ezg::Random ry(_area.getA().y, _area.getB().y);
        ezg::Random rz(_area.getA().z, _area.getB().z);

        return la::Vector3f( rx(), ry(), rz() );
    }

    static la::Triangle genTriangle(la::Rectangle3 _area)
    {
        return { genPoint3f(_area), genPoint3f(_area), genPoint3f(_area) };
    }


    template< typename T >
    static void gen_tr3_test_rand(std::basic_ostream< T >& out, size_t numTriangles, la::Rectangle3 area, float rad, size_t time)
    {
        out << numTriangles << '\n' << time << std::endl;

        ezg::Random rSpeed(-30, 30);

        for (size_t k = 0; k < numTriangles; )
        {
            la::Vector3f center = genPoint3f(area);
            const la::Rectangle3 loc_area({ center.x + rad, center.y + rad, center.z + rad },
                    { center.x - rad, center.y - rad, center.z - rad });

            const la::Triangle tr = genTriangle(loc_area);

            if (tr.valid()) {
                out << tr.getA().x << ' ';
                out << tr.getA().y << ' ';
                out << tr.getA().z << ' ';

                out << tr.getB().x << ' ';
                out << tr.getB().y << ' ';
                out << tr.getB().z << ' ';

                out << tr.getC().x << ' ';
                out << tr.getC().y << ' ';
                out << tr.getC().z << '\n';


                out << tr.getA().x << ' ';
                out << tr.getA().y << ' ';
                out << tr.getA().z << ' ';

                while(center == tr.getA()) {
                    center = genPoint3f(area);
                }
                out << center.x << ' ';
                out << center.y << ' ';
                out << center.z << '\n';


                out << rSpeed() << std::endl;


                k++;
            }
        }

    }

}//namespace ezg