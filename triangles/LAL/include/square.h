#pragma once

#include "config.h"
#include "Vector.h"

namespace la
{

	class Square
	{
		Vector3f m_a;
		Vector3f m_b;

	public:

		Square() = default;

		Square(Vector3f _a, Vector3f _b)
			: m_a(_a)
			, m_b(_b)
		{ 
			reup(_a, _b);
		}

		bool equal(const Square& _that) const noexcept;
		bool operator==(const Square& _that) const noexcept { return equal(_that); }

		Vector3f getA() const noexcept { return m_a; }
		Vector3f getB() const noexcept { return m_b; }

		double getDX() const noexcept { return m_b.x - m_a.x; }
		double getDY() const noexcept { return m_b.y - m_a.y; }
		double getDZ() const noexcept { return m_b.z - m_a.z; }

		void reup(const Vector3f& _a, const Vector3f& _b) noexcept;

		friend std::ostream& operator << (std::ostream& _stream, const Square& _sq);
	};

}//namespace la