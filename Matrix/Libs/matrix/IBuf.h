/*************************************************************************
 *
 * IBuf.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include <exception>
#include <iostream>
#include <string>

namespace matrix
{
	/*
	* class IBuff_t< T >
	* 
	* templates: 
	*	T - type of data
	* 
	* Allocates raw memory for subsequent object construction.
	*
	* Note: For proper operation, it is necessary to construct the objects in order (compact).
	* For each constructed object m_used++.
	*/
	template< typename T >
	class IBuff_t
	{
	public:

		IBuff_t             (const IBuff_t&) = delete;
		IBuff_t& operator = (const IBuff_t&) = delete;

	protected:

		T*     m_data = nullptr;

		//array size (raw memory)
		size_t m_size = 0;

		//number of constructed objects
		size_t m_used = 0;

	protected:

		IBuff_t() = default;
		
		explicit IBuff_t(size_t size)
			: m_data((size == 0) ? nullptr : static_cast< T* >(::operator new[](sizeof(T) * size)))
			, m_size(size)
			, m_used(0)
		{}

		~IBuff_t()
		{
			for (size_t k = 0; k < m_used; ++k)
			{
				(m_data + k)->~T();
			}
			::operator delete[] (m_data);
		}

		IBuff_t(IBuff_t&& that) noexcept
			: IBuff_t()
		{
			swap(that);
		}

		IBuff_t& operator = (IBuff_t&& that) noexcept
		{
			IBuff_t tmp(std::move(that));
			swap(tmp);
		}
		
		size_t size() const noexcept { return m_size; }

		void swap(IBuff_t& that) noexcept
		{
			std::swap(m_data, that.m_data);
			std::swap(m_size, that.m_size);
			std::swap(m_used, that.m_used);
		}
	};


}//namespace matrix