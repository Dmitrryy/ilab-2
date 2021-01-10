#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <string.h>

#include "MatrixBuff.h"

namespace matrix
{

	constexpr double EPSIL = 0.000000001;

	template <typename T = double>
	class Matrix : public MatrixBuffer_t< T >
	{
		//MatrixBuffer_t< T > m_buff;

	public:

        using MatrixBuffer_t<T>::begin;
        using MatrixBuffer_t<T>::end;
        using MatrixBuffer_t<T>::cbegin;
        using MatrixBuffer_t<T>::cend;

        using MatrixBuffer_t<T>::resize;
        using MatrixBuffer_t<T>::equal;
        using MatrixBuffer_t<T>::getLines;
        using MatrixBuffer_t<T>::getColumns;
        using MatrixBuffer_t<T>::getOrder;
        using MatrixBuffer_t<T>::clear;
        using MatrixBuffer_t<T>::at;
        using MatrixBuffer_t<T>::copy;
        using MatrixBuffer_t<T>::setOrder;
        using MatrixBuffer_t<T>::swap;
        using MatrixBuffer_t<T>::transpose;

        Matrix() = default;
		Matrix             (Matrix&&) noexcept = default;
		Matrix& operator=  (Matrix&&) noexcept = default;
		Matrix             (const Matrix& that) noexcept
		    : MatrixBuffer_t<T>(that)
        {}
		Matrix& operator=  (const Matrix& that) noexcept
        {
		    Matrix<T> tmp(that);
		    swap(tmp);
		    return *this;
        }

		Matrix(size_t num_lines, size_t num_column, Order order_ = Order::Row);

		Matrix(const std::initializer_list< std::initializer_list< T > >& list_, Order order_ = Order::Row);
		Matrix& operator=  (const std::initializer_list< std::initializer_list< T > >& list_);

		template <typename U>
		explicit Matrix(const Matrix<U>& that_) noexcept;


	public:

		T      trace() const noexcept;

		Matrix& add(const Matrix& rhs_)&;
		Matrix& sub(const Matrix& rhs_)&;
		Matrix& mul(const T& num_)&;

        void foldLines(size_t to, size_t what, const T& mult);
        void foldColumns(size_t to, size_t what, const T& mult);
        void mulLine(size_t what, const T& mult);
        void mulColumn(size_t what, const T& mult);

		Matrix& multiplication(const Matrix& that_)&;

		Matrix& negate()&;

		void swopLines(size_t l1_, size_t l2_);
		void swopColumns(size_t cl1_, size_t cl2_);

		Matrix<T> submatrix(size_t deleted_line, size_t deleted_column) const;

		T determinanteSloww() const;

		double determinante() const;

	public:

		static Matrix identity(size_t size_);

	public:

		T& operator () (size_t _y, size_t _x)&;
		const T& operator () (size_t _y, size_t _x) const&;

		Matrix<T>& operator += (const Matrix& that_)&;
		Matrix<T>& operator -= (const Matrix& that_)&;
		Matrix<T>& operator *= (const T& num_)&;

		template <typename U>
		bool operator != (const Matrix<U>& that_) const;

		friend std::ostream& operator << (std::ostream& stream_, const Matrix<T>& mtr_) {
			return stream_ << mtr_.dumpStr();
		}
	};//class Matrix


	template <typename T>
	Matrix<T>::Matrix(size_t num_lines, size_t num_column, Order order_ /*= Order::Row*/)
		: MatrixBuffer_t<T>(num_lines, num_column, order_)
	{	}


	template <typename T>
	Matrix<T>::Matrix(const std::initializer_list< std::initializer_list< T > >& list_, Order order_/* = Order::Row*/)
		: MatrixBuffer_t<T>(0, 0, order_)
	{
		size_t new_x = 0, new_y = list_.size();
		for (const auto& l : list_) {
			new_x = std::max(new_x, l.size());
		}

		resize(new_y, new_x);

		size_t cur_y = 0, cur_x = 0;
		for (const auto& str : list_) {
			for (const auto& elem : str)
			{
				at(cur_y, cur_x) = elem;
				cur_x++;
			}
			//std::cout << std::endl << *this << std::endl;
			cur_x = 0;
			cur_y++;
		}
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator = (const std::initializer_list< std::initializer_list< T > >& list_)
	{
        *this = std::move(Matrix(list_, getOrder()));
		return *this;
	}


	template <typename T>
    template <typename U>
    Matrix<T>::Matrix(const Matrix<U> &that_) noexcept
        : Matrix(that_.getLines(), that_.getColumns(), that_.getOrder())
    {
		copy(*this, that_);
    }

}//namespace matrix

#include "Matrix.inl"