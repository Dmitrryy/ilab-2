/**************************************************************************************************
 *
 *   main.c
 *
 *   Created by dmitry
 *   ?.?.2020
 *
 ***/

//
/// 1-determinant
///======================================================================================
/// This program calculates the determinant of the matrix using the Gaussian method.
///
/// At the input (via the standard stream), the size of the square matrix and then the
/// elements of this matrix.
///
/// In the output (via the standard stream), an integer is the determinant of the
/// corresponding matrix.
///======================================================================================
///======================================================================================
//

#include <iostream>
#include <cmath>
#include <vector>
#include "../Libs/matrix/Matrix.h"

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

    std::cout << std::round(m.determinante()) << std::endl;

	return 0;
}