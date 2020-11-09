#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <string.h>

namespace matrix
{
	enum class Order
	{
		Row
		, Column
	};

	constexpr double EPSIL = 0.000000001;

	template <typename T = double>
	class Matrix
	{
		T*     m_data     = nullptr;

		size_t m_capacity = 0;
		size_t m_size     = 0;
		size_t m_ncolumns = 0;
		size_t m_nlines   = 0;

		Order  m_order    = Order::Row;

	public:

		Matrix ();

		Matrix (size_t num_lines, size_t num_column, Order order_ = Order::Row);

		Matrix (const std::initializer_list< std::initializer_list< T > >& list_, Order order_ = Order::Row);
		Matrix& operator=  (const std::initializer_list< std::initializer_list< T > >& list_);

		Matrix             (const Matrix& that_) noexcept;
		Matrix& operator=  (const Matrix& that_) noexcept;

		template <typename U>
		Matrix             (const Matrix<U> &that_) noexcept;

		Matrix             (Matrix&& that_) noexcept;
		Matrix& operator=  (Matrix&& that_) noexcept;

		~Matrix () {
			delete[] m_data;
		}

	public:

		size_t getNLines  () const noexcept { return m_nlines; }
		size_t getNColumns() const noexcept { return m_ncolumns; }
		Order  getOrder   () const noexcept { return m_order; }
		T      trace      () const noexcept;

		void   resize(size_t y_, size_t x_);
		void   clear ();

		template <typename U>
		bool   equal (const Matrix<U>& that_) const;

		T&       at  (size_t y_, size_t x_) &;
		const T& at  (size_t y_, size_t x_) const&;

		void setOrder(Order order_);

		Matrix& add(const Matrix& rhs_)&;
		Matrix& sub(const Matrix& rhs_)&;
		Matrix& mul(const T& num_)&;

		Matrix& multiplication(const Matrix& that_)&;

		Matrix& transpose()&;
		Matrix& negate   ()&;

		std::string dumpStr () const;

		void swopLines  (size_t l1_, size_t l2_);
		void swopColumns(size_t cl1_, size_t cl2_);

		Matrix<T> submatrix (size_t deleted_column, size_t deleted_line) const;

		T determinanteSloww () const;

		double determinante () const;

	public:

	    static Matrix identity(size_t size_);

	public:

		T&       operator () (size_t _y, size_t _x)&;
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

	public:

		template <typename U>
		static void copy__(Matrix<T>& dest_, const Matrix<U>& source_, bool save_order_ = false);
	};


	template <typename T>
	Matrix<T>::Matrix()
	{
		//static_assert(!std::is_integral<T>::value, "trying to create a matrix of integers");
	}


	template <typename T>
	Matrix<T>::Matrix(size_t num_lines, size_t num_column, Order order_ /*= Order::Row*/)
		: m_data    (nullptr)
		, m_capacity(num_lines* num_column)
		, m_size    (num_lines* num_column)
		, m_ncolumns(num_column)
		, m_nlines  (num_lines)
		, m_order   (order_)
	{
		if (m_size != 0) {
			m_data = new T[num_lines * num_column]();
		}
		else {
			m_ncolumns = m_nlines = 0;
		}
	}


	template <typename T>
	Matrix<T>::Matrix(const std::initializer_list< std::initializer_list< T > >& list_, Order order_/* = Order::Row*/)
		: Matrix(0, 0, order_)
	{
		size_t new_y = 0, new_x = list_.size();
		for (const auto& l : list_) {
			new_y = std::max(new_y, l.size());
		}
		resize(new_y, new_x);

		size_t cur_y = 0, cur_x = 0;
		for (const auto& str : list_)
		{
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
		return *this = std::move(Matrix(list_, m_order));
	}


	template <typename T>
	Matrix<T>::Matrix(const Matrix& that_) noexcept
		: Matrix()
	{
        m_data = new T[that_.m_size];
        for (size_t i = 0; i < that_.m_size; i++)
        {
            m_data[i] = that_.m_data[i];
        }

        m_size = that_.m_size;
        m_capacity = that_.m_capacity;
        m_ncolumns = that_.m_ncolumns;
        m_nlines = that_.m_nlines;
        m_order = that_.m_order;
	}


	template <typename T>
    template <typename U>
    Matrix<T>::Matrix(const Matrix<U> &that_) noexcept
        : Matrix(that_.getNLines(), that_.getNColumns(), that_.getOrder())
    {
        copy__(*this, that_, true);
    }


	template <typename T>
	Matrix<T>& Matrix<T>::operator = (const Matrix& that_) noexcept
	{
		if (this == &that_)
			return *this;

		Matrix tmp(that_);
        *this = std::move(tmp);

        return *this;
	}


	template <typename T>
	Matrix<T>::Matrix(Matrix&& that_) noexcept
		: m_data    (nullptr)
		, m_capacity(that_.m_capacity)
		, m_size    (that_.m_size)
		, m_ncolumns(that_.m_ncolumns)
		, m_nlines  (that_.m_nlines)
		, m_order   (that_.m_order)
	{
		std::swap(m_data, that_.m_data);
        that_.m_size = that_.m_capacity
                = that_.m_ncolumns
                = that_.m_nlines = 0;
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator= (Matrix&& that_) noexcept {
		if (this == &that_)
			return *this;

		std::swap(m_data, that_.m_data);

		m_order = that_.m_order;
		m_size = that_.m_size;
		m_nlines = that_.m_nlines;
		m_ncolumns = that_.m_ncolumns;
		m_capacity = that_.m_capacity;

        that_.m_size = that_.m_capacity
                = that_.m_ncolumns
                = that_.m_nlines = 0;

        return *this;
	}



	std::string toString(Order order_);
	std::ostream& operator << (std::ostream& stream_, matrix::Order order_);

}//namespace matrix

#include "Matrix.inl"