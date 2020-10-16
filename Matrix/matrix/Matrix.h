#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <string.h>

namespace ezg
{

	template <typename T>
	class Matrix
	{
	public:

		enum class Order
		{
			  Row
			, Column
		};

	private:

		T*     m_data;

		size_t m_capacity;
		size_t m_size;
		size_t m_ncolumns;
		size_t m_nlines;

		Order  m_order;

	public:

		Matrix(size_t num_lines, size_t num_column, Order order_ = Order::Row)
			: m_data(new T[num_lines * num_column]())
			, m_capacity(num_lines * num_column)
			, m_size(num_lines * num_column)
			, m_ncolumns(num_column)
			, m_nlines(num_lines)
			, m_order(order_)
		{
			
		}

		Matrix()
			: m_data(nullptr)
			, m_capacity(0)
			, m_size(0)
			, m_ncolumns(0)
			, m_nlines(0)
			, m_order(Order::Row)
		{
			static_assert(!std::is_integral<T>::value, "trying to create a matrix of integers");
		}

		Matrix(const Matrix& that_)
		{
			copy__(*this, that_, false);
		}

		Matrix& operator= (const Matrix& that_)
		{
			if (this == &that_)
				return *this;

			copy__(*this, that_, false);
		}

		Matrix(Matrix&& that_)
		{
			std::swap(m_data, that_.m_data);

			m_order = that_.m_order;
			m_size = that_.m_size;
			m_nlines = that_.m_nlines;
			m_ncolumns = that_.m_ncolumns;
		}

		Matrix& operator= (Matrix&& that_)
		{
			if (this == &that_)
				return *this;

			std::swap(m_data, that_.m_data);

			m_order = that_.m_order;
			m_size = that_.m_size;
			m_nlines = that_.m_nlines;
			m_ncolumns = that_.m_ncolumns;
		}

		~Matrix() {
			delete[] m_data;
		}

	public:

		size_t getNLines  () const noexcept { return m_nlines; }
		size_t getNColumns() const noexcept { return m_ncolumns; }
		Order  getOrder   () const noexcept { return m_order; }

		void   resize(size_t y_, size_t x_);
		void   clear ();

		T&       at(size_t y_, size_t x_) &;
		const T& at(size_t y_, size_t x_) const&;

		void setOrder(Order order_);

		std::string dumpStr() const;

	private:

		static void copy__(Matrix<T>& dest_, const Matrix<T>& source_, bool save_order_ = false);
	};


	template <typename T>
	T& Matrix<T>::at(size_t y_, size_t x_)&
	{
		return const_cast< T& >(static_cast< const Matrix<T>* >(this)->at(y_, x_));
	}


	template <typename T>
	const T& Matrix<T>::at(size_t y_, size_t x_) const&
	{
		if (y_ >= m_nlines || x_ >= m_ncolumns) {
			throw std::out_of_range("Out of range valid value.");
		}

		if (m_order == Order::Row)
		{
			return *(m_data + y_ * m_ncolumns + x_);
		}
		else if (m_order == Order::Column)
		{
			return *(m_data + x_ * m_nlines + y_);
		}
		else { assert(0); }

	}


	template <typename T>
	void Matrix<T>::clear()
	{
		*this = std::move(Matrix<T>());
	}


	template <typename T>
	void Matrix<T>::setOrder(Order order_)
	{
		if (order_ != m_order)
		{
			Matrix<T> tmp(m_nlines, m_ncolumns, order_);
			copy__(tmp, *this, true);
			*this = std::move(tmp);
		}
	}


	template <typename T>
	void Matrix<T>::resize(size_t y_, size_t x_)
	{
		if (y_ == 0 || x_ == 0) {
			clear();
			return;
		}

		if (m_order == Order::Row && m_ncolumns == x_ && m_nlines >= y_)
		{
			m_nlines = y_;
		}
		else if (m_order == Order::Row && m_ncolumns == x_ && x_ * y_ <= m_capacity)
		{
			for (size_t i = 0; i < m_ncolumns; i++) {
				for (size_t k = m_nlines; k < y_; k++) {
					at(k, i) = T();
				}
			}
			m_nlines = y_;
		}
		else if (m_order == Order::Column && m_nlines == y_ && m_ncolumns >= x_)
		{
			m_ncolumns = x_;
		}
		else if (m_order == Order::Column && m_nlines == y_ && x_ * y_ <= m_capacity)
		{
			for (size_t i = 0; i < m_nlines; i++) {
				for (size_t k = m_ncolumns; k < x_; k++) {
					at(i, k) = T();
				}
			}
			m_ncolumns = x_;
		}
		else
		{
			Matrix<T> tmp(y_, x_, m_order);
			copy__(tmp, *this, true);
			*this = std::move(tmp);
		}
		m_size = x_ * y_;
	}


	template <typename T>
	std::string Matrix<T>::dumpStr() const
	{
		std::ostringstream out;

		out << "size: " << m_nlines << " x " << m_ncolumns << '\n'
			<< "order " << m_order << '\n'
			<< "Data:" << '\n';

		for (size_t y = 0; y < m_nlines; y++)
		{
			out << "| ";
			for (size_t x = 0; x < m_ncolumns; x++)
			{
				out << std::setw(6) << at(y, x) << ' ';
			}
			out << "|\n";
		}

		return out.str();
	}


	template <typename T>
	void Matrix<T>::
		copy__(Matrix<T>& dest_, const Matrix<T>& source_, bool save_order_/* = false*/)
	{
		const size_t min_y = std::min(source_.getNLines(), dest_.getNLines());
		const size_t min_x = std::min(source_.getNColumns(), dest_.getNColumns());

		if (save_order_) {
			if (source_.getOrder() == Order::Row) {
				for (size_t i = 0; i < min_y; i++)
				{
					for (size_t k = 0; k < min_x; k++)
					{
						dest_.at(i, k) = source_.at(i, k);
					}
				}
			}
			else if (source_.getOrder() == Order::Column) {
				for (size_t i = 0; i < min_x; i++)
				{
					for (size_t k = 0; k < min_y; k++)
					{
						dest_.at(k, i) = source_.at(k, i);
					}
				}
			}
			else {
				assert(0);
			}
		}
		else
		{
			dest_.setOrder(source_.getOrder());
			copy__(dest_, source_, true);
		}
	}


#define orCase(a) \
case Matrix<T>::Order::a: \
res = #a; \
break;

	template <typename T = float>
	std::string toString(typename Matrix<T>::Order order_)
	{
		std::string res;

		switch (order_)
		{
			orCase(Row);
			orCase(Column);
		}

		return res;
	}

	template <typename T = float>
	std::ostream& operator << (std::ostream& stream_, typename Matrix<T>::Order order_)
	{
		return stream_ << toString(order_);
	}

}//namespace ezg