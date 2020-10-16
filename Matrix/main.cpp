#include <iostream>

#include "matrix/Matrix.h"

int main()
{
	ezg::Matrix<float> m(4, 4);

	m.at(2, 0) = 1;
	m.at(1, 2) = 2;
	m.at(3, 3) = 3;
	m.resize(5, 5);


	m.setOrder(ezg::Matrix<float>::Order::Column);
	m.setOrder(ezg::Matrix<float>::Order::Row);

	std::cout << m.dumpStr();

	return 0;
}