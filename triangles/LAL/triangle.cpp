#include <iostream>

#include "include/triangle.h"
#include "include/Plane.h"
#include "include/LALmath.h"

#include <vector>
#include <algorithm>

namespace la
{

	Plane Triangle::getPlane() const noexcept 
	{ 
		return { m_a, m_b, m_c, Plane::Type::ThreePoints }; 
	}

	bool Triangle::equal(const Triangle& _that) const noexcept
	{
		return m_a == _that.getA() && m_b == _that.getB() && m_c == _that.getC();
	}

	void Triangle::reup(Vector3f _a, Vector3f _b, Vector3f _c)
	{
		std::vector<Vector3f> cnt = {_a, _b, _c};

		std::sort(cnt.begin(), cnt.end(), [](auto _lhs, auto _rhs)
			{
				return _lhs.modul() < _rhs.modul();
			});

		m_a = cnt[0];
		m_b = cnt[1];
		m_c = cnt[2];
	}

	std::ostream& operator << (std::ostream& _stream, const Triangle& _tar)
	{
		_stream << '[' << _tar.m_a << ", " << _tar.m_b << ", " << _tar.m_c << ']';
		return _stream;
	}

	bool Triangle::contein(const Vector3f& _vec) const noexcept
	{
		bool res = false;
		
		if (la::contein(getPlane(), _vec))
		{
			res =         distance(_vec, Line3(m_a, m_b)) <= distance(m_c, Line3(m_a, m_b));
			res = res  && distance(_vec, Line3(m_c, m_b)) <= distance(m_a, Line3(m_c, m_b));
			res = res  && distance(_vec, Line3(m_c, m_a)) <= distance(m_b, Line3(m_c, m_a));
		}

		return res;
	}
} // namespace la