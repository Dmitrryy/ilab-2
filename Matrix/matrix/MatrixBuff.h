#pragma once

#include <cassert>
#include <functional>
#include <sstream>

#include "IBuf.h"

namespace matrix
{

	enum class Order
	{
		Row
		, Column
	};

	template< typename T >
	class MatrixBuffer_t : private IBuff_t< T >
	{
		size_t m_lines = 0, m_columns = 0;
		Order m_order = Order::Row;

		using IBuff_t<T>::m_used;
		using IBuff_t<T>::m_size;
		using IBuff_t<T>::m_data;

	public:

		MatrixBuffer_t() = default;
		MatrixBuffer_t(size_t lines, size_t columns, Order order = Order::Row);


		MatrixBuffer_t(const MatrixBuffer_t& that);
		MatrixBuffer_t& operator = (const MatrixBuffer_t& that);


		template< typename U >
		MatrixBuffer_t(const MatrixBuffer_t< U >& that);


		MatrixBuffer_t(MatrixBuffer_t&& that) noexcept;
		MatrixBuffer_t& operator = (MatrixBuffer_t&& that) noexcept;

	public:

		////////////////////get////////////////////////////
		Order  getOrder  () const noexcept { return m_order; }
		size_t getLines  () const noexcept { return m_lines; }
		size_t getColumns() const noexcept { return m_columns; }
		///////////////////////////////////////////////////

		void setOrder(Order nOrder);

		MatrixBuffer_t& transpose()&;

		const T& at(size_t line, size_t column) const&;
		T& at(size_t lines, size_t column)& { return const_cast< T& >(static_cast< const MatrixBuffer_t* >(this)->at(lines, column)); }

		void swap(MatrixBuffer_t& that) noexcept;

		void clear();

		void forAll(std::function< bool(T&, size_t, size_t) >& func);

		template< typename U >
		static void copy(MatrixBuffer_t< T >& dest, const MatrixBuffer_t< U >& src);

		void resize(size_t lines, size_t columns);

	private:

		std::string dumpStr() const;
		
	};

	std::string toString(Order order_);
	std::ostream& operator << (std::ostream& stream_, matrix::Order order_);

}//namespace matrix


#include "MatrixBuffer.inl"