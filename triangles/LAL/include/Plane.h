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

		//r = _point + a * _vec1 + b * _vec2
		Plane(Vector3f _point, Vector3f _vec1, Vector3f _vec2, Type _t = Type::PointAndTwoVec) noexcept
			: m_p(_point)
			, m_a(_vec1)
			, m_b(_vec2)
		{
			reup(_point, _vec1, _vec2, _t);
		}

		bool valid   () const noexcept;

		Vector3f getP() const noexcept { return m_p;    }
		Vector3f getA() const noexcept { return m_a;    }
		Vector3f getB() const noexcept { return m_b;    }
		Vector3f getN() const noexcept { return m_norm; }

		bool operator == (const Plane& _that) const noexcept;

		void reup (Vector3f _point, Vector3f _vec1, Vector3f _vec2, Type _t = Type::PointAndTwoVec) noexcept;
		
		std::string dump() const;

		friend std::ostream& operator << (std::ostream& _stream, const Plane& _target) {
			return _stream << _target.dump();
		}
	};

}//namespace ezg