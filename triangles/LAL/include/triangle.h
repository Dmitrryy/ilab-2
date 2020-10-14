#pragma once

#include <cmath>
#include <cassert>

#include "config.h"
#include "Vector.h"
#include "square.h"

namespace la
{

	class Triangle
	{
		Vector3f m_a;
		Vector3f m_b;
		Vector3f m_c;

	public:

		Triangle() = default;

		Triangle(Vector3f _a, Vector3f _b, Vector3f _c)
			: m_a(_a)
			, m_b(_b)
			, m_c(_c)
		{
			reup(_a, _b, _c);
		}

	public:

		void     reup (Vector3f _a, Vector3f _b, Vector3f _c);

		bool valid() const noexcept;

		Vector3f getA () const noexcept { return m_a; }
		Vector3f getB () const noexcept { return m_b; }
		Vector3f getC () const noexcept { return m_c; }

		Plane getPlane() const noexcept;

		bool   contein(const Vector3f& _vec) const;

		Square getArea() const noexcept;

		bool   equal  (const Triangle& _that) const noexcept;

		bool operator == (const Triangle& _that) const noexcept { return equal(_that); }

		std::string dump () const;

		friend std::ostream& operator << (std::ostream& _stream, const Triangle& _tar) {
			return _stream << _tar.dump();
		}
	};
   
}//namespace la