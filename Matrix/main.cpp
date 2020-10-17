#include <iostream>

#include "matrix/Matrix.h"
#include "tests/gen_test.h"

using matrix::Matrix;
using matrix::Order;

int main()
{
	//size_t mat_size = 0;
	//std::cin >> mat_size;
	//Matrix<double> m(mat_size, mat_size);
	//for (size_t i = 0; i < mat_size; i++) {
	//	for (size_t l = 0; l < mat_size; l++) {
	//		std::cin >> m.at(i, l);
	//	}
	//}
	//std::cout << m.determinante() << std::endl;

	//please!!! size < 11!!!!!!

	Matrix s(3, 3);
	Matrix d(3, 3);
	Matrix f(3, 3);

	f = (s + d) + (f + d);

	RunTest(1000, 5);

	return 0;
}