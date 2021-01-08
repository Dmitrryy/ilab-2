#include <iostream>
#include <fstream>

#include <matrix/Matrix.h>
#include "gen_test.h"

#define TEST_IN_STREAM
//#define GEN_TXT_TEST

using matrix::Matrix;
using matrix::Order;

#ifdef GEN_TESTS_MAIN

int main()
{
#ifdef GEN_TXT_TEST
    long long int det = 32;
    size_t mat_size = 100;
    Matrix<double> m = genMatrix(mat_size, MType::MulUpLowTriangles, &det);
    std::cout << m << std::endl << det << std::endl;
    std::cout << static_cast<long long int>(m.determinante()) << std::endl;

    {
        std::ofstream out("0003.txt");
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
        std::ofstream out("0003tr.txt");
        out << det;
        out.close();
    }
#endif //GEN_TXT_TEST

#ifdef TEST_IN_STREAM

    std::ifstream input("tests/1001.txt");

    if (!input.is_open()) {
        std::cout << "nop" << std::endl;
        return 1;
    }
    size_t N = 0;
    input >> N;

    Matrix<double> matr(N, N, Order::Row);
    for (size_t l = 0; l < N; l++)
    {
        for (size_t c = 0; c < N; c++)
        {
            input >> matr.at(l, c);
        }
    }

    std::cout << std::endl << std::round(matr.determinante()) << std::endl;

#endif

    return 0;
}

#endif