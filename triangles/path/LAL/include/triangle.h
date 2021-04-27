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

		Rectangle3 getArea() const noexcept 
		{ 
			using std::min;
			using std::max;

			Vector3f a, b;
			a.x = min(getA().x, min(getB().x, getC().x));
			a.y = min(getA().y, min(getB().y, getC().y));
			a.z = min(getA().z, min(getB().z, getC().z));
			b.x = max(getA().x, max(getB().x, getC().x));
			b.y = max(getA().y, max(getB().y, getC().y));
			b.z = max(getA().z, max(getB().z, getC().z));

			return Rectangle3{ a, b };
		}

		bool   equal(const Triangle& _that) const noexcept;
		bool   intersec  (const Triangle& _that) const noexcept;

		bool operator == (const Triangle& _that) const noexcept { return equal(_that); }

		std::string dump () const;

		friend std::ostream& operator << (std::ostream& _stream, const Triangle& _tar) {
			return _stream << _tar.dump();
		}
	};
   
}//namespace la