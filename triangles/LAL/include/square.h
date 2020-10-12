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

		Vector3f getA() const noexcept { return m_a; }
		Vector3f getB() const noexcept { return m_b; }

		void reup(const Vector3f& _a, const Vector3f& _b) noexcept;
	};

}//namespace la