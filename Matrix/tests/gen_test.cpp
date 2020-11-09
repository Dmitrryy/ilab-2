#include "gen_test.h"
#include "../matrix/Matrix.h"

#include <fstream>

using namespace matrix;

void RunTest(size_t num_tests, size_t mat_size)
{
	Random r(-5, 5);

	Matrix<double> m{ mat_size, mat_size };
	size_t count_wrong = 0;
	for (size_t k = 0; k < num_tests; k++)
	{

		for (size_t i = 0; i < mat_size; i++) {
			for (size_t l = 0; l < mat_size; l++) {
				m.at(i, l) = r();
			}
		}

		const double res1 = m.determinante();
		const double res2 = m.determinanteSloww();
		//double res2 = res1;

		if (std::abs(res1 - res2) > EPSIL)
		{
			std::string fname = std::string("tests/bad") + std::to_string(k) + ".txt";
			std::ofstream out(fname, std::ios::trunc);
			out << mat_size << '\n';
			for (size_t i = 0; i < mat_size; i++) {
				for (size_t l = 0; l < mat_size; l++) {
					out << m.at(i, l) << ' ';
				}
			}
			count_wrong++;
			std::cout << "bad test: " << fname << " || " << res1 << "!=" << res2 << std::endl;
			//break;
		}
		else
		{
			std::cout << "OK: " << k << std::endl;
		}
	}
	std::cout << "miss: " << count_wrong << std::endl;

}


Matrix<double> genMatrix(size_t size_, MType type_, long long int *det_ /*= nullptr*/)
{
    Matrix<double> res(size_, size_);
    Random r(-50, 50);

    if (type_ == MType::Diagonal)
    {
        if (det_ != nullptr && *det_ != 0) {
            res = Matrix<double>::identity(size_);
            Random r2(0, size_);
            size_t pos = r2();
            res.at(pos, pos) = *det_;
        }
        else {
            if (det_ != nullptr) { *det_ = 1; }
            for (size_t i = 0u; i < size_; ++i) {
                res(i, i) = r();
                if (det_ != nullptr) { *det_ *= res(i, i); }
            }
        }
    }
    else if (type_ == MType::LowTriangle)
    {
        res = genMatrix(size_, MType::Diagonal, det_);
        for (size_t i = 0u; i < size_; ++i)
        {
            for (size_t k = 0u; k < i; ++k)
            {
                res(i, k) = r();
            }
        }
    }
    else if (type_ == MType::UpTriangle)
    {
        res = genMatrix(size_, MType::LowTriangle, det_);
        res.transpose();
    }
    else if (type_ == MType::MulUpLowTriangles)
    {
        long long det1 = 1;
        long long det2 = (det_ == nullptr) ? 0 : *det_;

        Matrix<double> low = genMatrix(size_, MType::LowTriangle, (det_ == nullptr) ? nullptr : &det1);
        //std::cout << low << std::endl;
        Matrix<double> up = genMatrix(size_, MType::UpTriangle, (det_ == nullptr) ? nullptr : &det2);
        //std::cout << up << std::endl;

        res = low.multiplication(up);
        if (det_ != nullptr) {
            *det_ = det1 * det2;
        }
    }

    return res;
}