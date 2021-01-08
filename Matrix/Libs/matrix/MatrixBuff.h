#pragma once

#include <cassert>
#include <functional>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "IBuf.h"
#include <iterator>

namespace matrix
{
    template <typename Pointer, typename Conteiner>
    class normal_iterator_t;

	enum class Order
	{
		Row
		, Column
	};

	template< typename T >
	class MatrixBuffer_t : private IBuff_t< T >
	{
		size_t m_lines   = 0;
		size_t m_columns = 0;
		Order  m_order = Order::Row;

		using IBuff_t<T>::m_used;
		using IBuff_t<T>::m_size;
		using IBuff_t<T>::m_data;

	public:

	    using value = T;
	    using const_value = const T;

	    using iterator = normal_iterator_t< value, MatrixBuffer_t< T > >;
	    using const_iterator = normal_iterator_t< const_value, const MatrixBuffer_t< T > >;


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

        template <typename U>
        bool   equal(const MatrixBuffer_t<U>& that_) const;

        template <typename U>
        bool operator == (const MatrixBuffer_t<U>& that_) const { return equal(that_); }

		iterator begin() noexcept { return iterator(m_data, m_data + m_used, m_data, 0, 0); }
		iterator end() noexcept { return iterator(m_data, m_data + m_used, m_data + m_used, getLines() - 1, getColumns() - 1); }

		const_iterator cbegin() const noexcept { return const_iterator(m_data, m_data + m_used, m_data, 0, 0); }
		const_iterator cend() const noexcept { return const_iterator(m_data, m_data + m_used, m_data + m_used, getLines() - 1, getColumns() - 1); }

		void setOrder(Order nOrder);

		MatrixBuffer_t& transpose()&;

		const T& at(size_t line, size_t column) const&;
		T& at(size_t lines, size_t column)& { return const_cast< T& >(static_cast< const MatrixBuffer_t* >(this)->at(lines, column)); }

		void swap(MatrixBuffer_t& that) noexcept;

		void clear();

		template< typename U >
		static void copy(MatrixBuffer_t< T >& dest, const MatrixBuffer_t< U >& src);

		void resize(size_t lines, size_t columns);

	//private:

		std::string dumpStr() const;

        friend std::ostream& operator << (std::ostream& stream_, const MatrixBuffer_t<T>& mtr_) {
            return stream_ << mtr_.dumpStr();
        }
		
	};//class MatrixBuffer_t


	template<typename T>
	struct Range
    {
	    T left, right;

	    Range(const T& nLeft, const T& nRight)
	        : left(nLeft)
	        , right(nRight)
        { }

	    bool contain(const T& that) const
        {
	        return that >= left && that <= right;
        }
    };//struct Range


	template <typename T>
	struct Elem
    {
	    T& val;

		size_t line = 0;
		size_t column = 0;


	    Elem(T& elem, size_t l, size_t c)
	        : val(elem)
	        , line(l)
	        , column(c)
        {}

        operator T&() { return val; }

        template <typename U>
        Elem& operator = (const U& that) {
	        val = that;
	        return *this;
	    }
    };//struct Elem



	template< typename Val, typename Conteiner >
    class normal_iterator_t : public std::iterator< std::forward_iterator_tag, typename Conteiner::value >
    {
        Val* m_value                  = nullptr;
        Range< Val* > m_range;

        size_t cur_line = 0, cur_column = 0;

        Val m_negateVal = Val();

    public:

        using elem_t = Elem< Val >;

        normal_iterator_t() = default;
        normal_iterator_t(Val* left, Val* right, Val* val, size_t cLine, size_t cColumn)
            : m_range(left, right)
            , m_value(val)
            , cur_column(cColumn)
            , cur_line(cLine)
        {}

        Elem< Val > operator *  () const noexcept {
            if (m_range.contain(m_value))
                return Elem< Val >(*m_value, cur_line, cur_column);
            else
                return Elem< Val >((Val&)(m_negateVal), 0, 0);
        }
        Val* operator -> () const noexcept { return m_value; }


        normal_iterator_t& operator++() noexcept {
            m_value++;
            return *this;
        }
        const normal_iterator_t operator++(int) noexcept {
            normal_iterator_t tmp(*this);
            ++*this;
            return tmp;
        }

        normal_iterator_t& operator--() noexcept {
            m_value--;
            return *this;
        }

        const normal_iterator_t operator--(int) noexcept {
            normal_iterator_t tmp(*this);
            --*this;
            return tmp;
        }


        bool operator == (const normal_iterator_t& that) const noexcept {
            return m_value == that.m_value;
        }
        bool operator != (const normal_iterator_t& that) const noexcept {
            return !(*this == that);
        }

    };//class normal_iterator_t


	std::string toString(Order order_);
	std::ostream& operator << (std::ostream& stream_, matrix::Order order_);

}//namespace matrix


#include "MatrixBuffer.inl"