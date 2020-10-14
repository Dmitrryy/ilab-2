#include <iostream>

#include "include/triangle.h"
#include "include/Plane.h"
#include "include/LALmath.h"

#include <vector>
#include <algorithm>
#include <sstream>

namespace la
{
	bool Triangle::valid() const noexcept
	{
		return m_a.valid() && m_b.valid() && m_c.valid()
			&& !(m_a - m_b).collinear(m_a - m_c);
	}

	Plane Triangle::getPlane() const noexcept 
	{ 
		return { m_a, m_b, m_c, Plane::Type::ThreePoints }; 
	}

	Square Triangle::getArea() const noexcept
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

		return { a, b };
	}

	bool Triangle::equal(const Triangle& _that) const noexcept
	{
		return m_a == _that.getA() && m_b == _that.getB() && m_c == _that.getC();
	}

	void Triangle::reup(Vector3f _a, Vector3f _b, Vector3f _c)
	{
		std::vector<Vector3f> cnt = {_a, _b, _c};

		std::sort(cnt.begin(), cnt.end(), [](auto _lhs, auto _rhs) noexcept
			{
				return _lhs.modul() < _rhs.modul();
			});

		m_a = cnt.at(0);
		m_b = cnt.at(1);
		m_c = cnt.at(2);
	}

	bool Triangle::contein(const Vector3f& _vec) const
	{
		bool res = false;
		
		if (la::contein(getPlane(), _vec))
		{
			if (LineSegment3(m_a, m_b).contein(_vec) ||
				LineSegment3(m_b, m_c).contein(_vec) ||
				LineSegment3(m_c, m_a).contein(_vec)) 
			{ 
				res = true;
			}
			else
			{
				const Vector3f p1 = m_a + (((m_c + ((m_b - m_c) / 2.f)) - m_a) * 3.f / 5.f);
				const LineSegment3 p1_vec(p1, _vec);

				res = findIntersec(LineSegment3(m_b, m_a), p1_vec).second == Intersec::quantity::Nop;
				res = res && findIntersec(LineSegment3(m_b, m_c), p1_vec).second == Intersec::quantity::Nop;
				res = res && findIntersec(LineSegment3(m_a, m_c), p1_vec).second == Intersec::quantity::Nop;
			}
		}

		return res;
	}

	std::string Triangle::dump() const
	{
		std::ostringstream out;
		out << "A: " << m_a << " B: " << m_b << " C: " << m_c;
		return out.str();
	}

} // namespace la