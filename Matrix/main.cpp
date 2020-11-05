#include <iostream>

#include <Matrix.h>

using matrix::Matrix;
using matrix::Order;

int main()
{
	size_t mat_size = 0;
	std::cin >> mat_size;
	Matrix<double> m(mat_size, mat_size);
	for (size_t i = 0; i < mat_size; i++) {
		for (size_t l = 0; l < mat_size; l++) {
			std::cin >> m.at(i, l);
		}
	}
	std::cout << static_cast<long long int>(m.determinante()) << std::endl;

	return 0;
}