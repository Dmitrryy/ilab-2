#pragma once

#include <vector>
#include <array>

#include "../LAL/include/LALmath.h"

namespace la
{

	template < typename T >
	class Octree
	{
		std::vector<T> m_data;

		Square m_area;

		size_t m_deep;

		std::array< Octree< T >*, 8 > m_children;

	public:

		Octree()
			: m_data()
			, m_a(0.0)
			, m_b(0.0)
			, m_deep(0)
			, m_children{nullptr}
		{}

		~Octree() {
			for (int i = 0; i < 8; i++)
				delete m_children[i];
		}

		size_t split  (size_t _deep);
		size_t msplit ();

		void add(const T& _elem);

		std::vector<T> getIntersections() const;
	};

}//namespace la