#include "include/Plane.h"
#include "include/Line.h"

#include "include/LALmath.h"
namespace la
{
	void Plane::reup(Vector3f _point, Vector3f _vec1, Vector3f _vec2, Type _t /*= Type::PointAndTwoVec*/)
	{
		m_p = _point;

		if (_t == Type::ThreePoints)
		{
			m_a = _vec1 - _point;
			m_b = _vec2 - _point;
		}
		else if (_t == Type::PointAndTwoVec)
		{
			m_a = _vec1;
			m_b = _vec2;
		}
		m_norm = crossProduct(m_a, m_b);
		m_norm = normalization(m_norm);
	}

	void Plane::reup(float _a, float _b, float _c, float _d)
	{
		if (_a > EPSILON)
		{
			m_a = Vector3f(-_b / _a, 1.f, 0.f);
			m_b = Vector3f(-_c / _a, 0.f, 1.f);

			m_p = Vector3f(-_d / _a, 0.f, 0.f);
		}
		else if (_b > EPSILON)
		{
			m_a = Vector3f(1.f, 0.f, 0.f);
			m_b = Vector3f(0.f, -_c / _b, 1.f);

			m_p = Vector3f(1.f, 0.f, 0.f);
		}
		else if (_c > EPSILON)
		{
			m_a = Vector3f(1.f, 0.f, 0.f);
			m_b = Vector3f(0.f, 1.f, 0.f);

			m_p = Vector3f(0.f, 0.f, -_d / _c);
		}

		m_norm = Vector3f(_a, _b, _c);
		m_norm = normalization(m_norm);
	}

}//namespace la