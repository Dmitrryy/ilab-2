#pragma once

#include "config.h"
#include "Vector.h"

namespace la
{

	class Plane
	{
		Vector3f m_p;
		Vector3f m_a;
		Vector3f m_b;

		Vector3f m_norm;

	public:

		enum class Type
		{
			ThreePoints
			, PointAndTwoVec
		};
		Plane() = default;

		//Ax + By + Cz + D = 0
		Plane(float _a, float _b, float _c, float _d)
		{
			reup(_a, _b, _c, _d);
		}

		//r = _point + a * _vec1 + b * _vec2
		Plane(Vector3f _point, Vector3f _vec1, Vector3f _vec2, Type _t = Type::PointAndTwoVec)
			: m_p(_point)
			, m_a(_vec1)
			, m_b(_vec2)
		{
			reup(_point, _vec1, _vec2, _t);
		}

		Vector3f getP() const noexcept { return m_p; }
		Vector3f getA() const noexcept { return m_a; }
		Vector3f getB() const noexcept { return m_b; }
		Vector3f getN() const noexcept { return m_norm; }

		void reup(Vector3f _point, Vector3f _vec1, Vector3f _vec2, Type _t = Type::PointAndTwoVec);
		void reup(float _a, float _b, float _c, float _d);
	};

}//namespace ezg