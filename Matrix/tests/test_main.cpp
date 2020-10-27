#include <iostream>
#include <fstream>

#include <Matrix.h>
#include <gen_test.h>

using matrix::Matrix;
using matrix::Order;

int main()
{
    long long int det = 0.0;
    size_t mat_size = 5;
    Matrix<double> m = genMatrix(mat_size, MType::LowTriangle, &det);
    std::cout << m << std::endl << det << std::endl;
    std::cout << static_cast<long long int>(m.determinante()) << std::endl;

    {
        std::ofstream out("0008.txt");
        if (!out.is_open()) { return 13; }
        out << mat_size << '\n';
        for (size_t i = 0; i < mat_size; i++) {
            for (size_t k = 0; k < mat_size; k++) {
                out << m(i, k) << ' ';
            }
            out << '\n';
        }
        out.close();
    }

    {
        std::ofstream out("0008tr.txt");
        out << det;
        out.close();
    }
    //RunTest(1000, 5);

    return 0;
}