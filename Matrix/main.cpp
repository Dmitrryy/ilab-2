#include <iostream>

#include "matrix/Matrix.h"
#include "tests/Matrix_test.h"

using matrix::Matrix;
using matrix::Order;

int main()
{
	Matrix<double> m(4, 4);
	Matrix<double> y(4, 4);

	Matrix<double> c(6, 6, Order::Column);

	c = {
		{1, 2},
		{2, 0}
	};

	m.at(2, 0) = 1;
	m.at(1, 2) = 2;
	m.at(3, 3) = 3;
	m.resize(5, 5);


	m.setOrder(Order::Column);
	m.setOrder(Order::Row);

	std::cout << m.dumpStr();

	return 0;
}