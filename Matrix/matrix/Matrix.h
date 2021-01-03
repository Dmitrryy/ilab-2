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
	class Matrix
	{
		MatrixBuffer_t< T > m_buff;

	public:

		Matrix() = default;
		Matrix             (Matrix&&)      = default;
		Matrix& operator=  (Matrix&&)      = default;
		Matrix             (const Matrix&) = default;
		Matrix& operator=  (const Matrix&) = default;

		Matrix(size_t num_lines, size_t num_column, Order order_ = Order::Row);

		Matrix(const std::initializer_list< std::initializer_list< T > >& list_, Order order_ = Order::Row);
		Matrix& operator=  (const std::initializer_list< std::initializer_list< T > >& list_);

		template <typename U>
		Matrix(const Matrix<U>& that_) noexcept;

	public:

		////////////////////get////////////////////////////
		Order  getOrder()   const noexcept { return m_buff.getOrder(); }
		size_t getLines()   const noexcept { return m_buff.getLines(); }
		size_t getColumns() const noexcept { return m_buff.getColumns(); }
		///////////////////////////////////////////////////

		void forAll(std::function< bool(T&, size_t, size_t) >& func) { m_buff.forAll(func);  }

		T      trace() const noexcept;

		void   resize(size_t y_, size_t x_) { m_buff.resize(y_, x_); }
		void   clear();

		template <typename U>
		bool   equal(const Matrix<U>& that_) const;

		T& at(size_t y_, size_t x_)& { return const_cast<T&>(static_cast<const Matrix<T>*>(this)->at(y_, x_)); }
		const T& at(size_t y_, size_t x_) const& { return m_buff.at(y_, x_); }

		void setOrder(Order order_) { m_buff.setOrder(); }

		Matrix& add(const Matrix& rhs_)&;
		Matrix& sub(const Matrix& rhs_)&;
		Matrix& mul(const T& num_)&;

		Matrix& multiplication(const Matrix& that_)&;

		Matrix& transpose()& { m_buff.transpose(); /**/ return *this; }
		Matrix& negate()&;

		std::string dumpStr() const { return m_buff.dumpStr(); }

		void swopLines(size_t l1_, size_t l2_);
		void swopColumns(size_t cl1_, size_t cl2_);

		Matrix<T> submatrix(size_t deleted_column, size_t deleted_line) const;

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
		bool operator == (const Matrix<U>& that_) const;

		template <typename U>
		bool operator != (const Matrix<U>& that_) const;

		friend std::ostream& operator << (std::ostream& stream_, const Matrix<T>& mtr_) {
			return stream_ << mtr_.dumpStr();
		}
	};//class Matrix


	template <typename T>
	Matrix<T>::Matrix(size_t num_lines, size_t num_column, Order order_ /*= Order::Row*/)
		: m_buff(num_lines, num_column, order_)
	{	}


	template <typename T>
	Matrix<T>::Matrix(const std::initializer_list< std::initializer_list< T > >& list_, Order order_/* = Order::Row*/)
		: Matrix(0, 0, order_)
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
			cur_x = 0;
			cur_y++;
		}
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator = (const std::initializer_list< std::initializer_list< T > >& list_)
	{
		return *this = std::move(Matrix(list_, getOrder()));
	}


	template <typename T>
    template <typename U>
    Matrix<T>::Matrix(const Matrix<U> &that_) noexcept
        : Matrix(that_.getLines(), that_.getColumns(), that_.getOrder())
    {
		MatrixBuffer_t< T >::copy(*this, that_);
    }

}//namespace matrix

#include "Matrix.inl"