
#include <circuit/Circuit.h>
#include <iostream>

namespace ezg
{

    void genTest(size_t x, size_t y, float eds, std::ostream& test_out, std::ostream& ans_out)
    {
        const size_t vert_x = x + 2;
        const float res_line = 2 + (x - 1) * 2;
        const float current = eds / res_line;

        for (size_t cy = 1; cy <= y; cy++)
        {
            for (size_t cx = 1; cx <= x + 2; cx++)
            {
                if (cx != x + 2) {
                    test_out << vert_x * (cy - 1) + cx << " -- " << vert_x * (cy - 1) + cx + 1 << ", "
                             << 1 + ((cx == 1 || cx == vert_x - 1) ? 0 : 1) << ";\n";

                    ans_out << vert_x * (cy - 1) + cx << " -- " << vert_x * (cy - 1) + cx + 1 << ": "
                             << current << " A\n";
                }

                if (cy != y) {
                    test_out << vert_x * (cy - 1) + cx << " -- " << vert_x * (cy) + cx << ", "
                             << ((cx == 1 || cx == vert_x) ? 0 : 2) << ";\n";

                    ans_out << vert_x * (cy - 1) + cx << " -- " << vert_x * (cy) + cx << ": "
                            << ((cx == 1 || cx == vert_x) ? (cy * current * ((cx == 1) ? -1.f : 1.f)) : 0.f) << " A\n";
                }
            }
        }

        test_out << (y - 1) * vert_x + 1 << " -- " << y * vert_x + 1 << ", 0.0;\n";
        ans_out << (y - 1) * vert_x + 1 << " -- " << y * vert_x + 1 << ": " << current * y * -1.f << " A\n";

        test_out << (y) * vert_x << " -- " << (y + 1) * vert_x << ", 0.0;\n";
        ans_out << (y) * vert_x << " -- " << (y + 1) * vert_x << ": " << current * y << " A\n";

        test_out << y * vert_x + x + 2 << " -- " << y * vert_x + 1<< ", 0.0; " << eds << " V\n";
        ans_out << y * vert_x + x + 2 << " -- " << y * vert_x + 1 << ": " << current * y << " A\n";


   }

}//namespace ezg
