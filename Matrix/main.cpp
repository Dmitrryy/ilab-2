
#include <iostream>
#include <cmath>
#include <vector>
#include <Matrix.h>

using matrix::Matrix;
using matrix::Order;

void foo(int t)
{
    std::cout << t << std::endl;
}

int main()
{
/*	size_t mat_size = 0;
	std::cin >> mat_size;
	Matrix<double> m(mat_size, mat_size);
	for (size_t i = 0; i < mat_size; i++) {
		for (size_t l = 0; l < mat_size; l++) {
			std::cin >> m.at(i, l);
		}
	}
    std::cout << std::round(m.determinante()) << std::endl;*/
	int v = 4;

    matrix::Elem<int> t(v, 1, 1);
    foo(t);
    t = 5;


	return 0;
}