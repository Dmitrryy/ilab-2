#pragma once

#include "config.h"
#include "Vector.h"

namespace la
{

	class Rectangle3
	{
		Vector3f m_a;
		Vector3f m_b;

	public:

		Rectangle3() = default;

		Rectangle3(Vector3f _a, Vector3f _b) noexcept
			: m_a(_a)
			, m_b(_b)
		{ 
			reup(_a, _b);
		}

		bool valid () const noexcept;
		bool equal (const Rectangle3& _that) const noexcept;

		bool operator == (const Rectangle3& _that) const noexcept { return equal(_that); }

		Vector3f getA() const noexcept { return m_a; }
		Vector3f getB() const noexcept { return m_b; }

		double getDX () const noexcept { return m_b.x - m_a.x; }
		double getDY () const noexcept { return m_b.y - m_a.y; }
		double getDZ () const noexcept { return m_b.z - m_a.z; }

		void reup    (const Vector3f& _a, const Vector3f& _b) noexcept;

		std::string dump() const;

		friend std::ostream& operator << (std::ostream& _stream, const Rectangle3& _sq) {
			return _stream << _sq.dump();
		}
	};

	class Rectangle2
	{
		Vector2f m_a;
		Vector2f m_b;

	public:

		Rectangle2() = default;

		Rectangle2(Vector2f a_, Vector2f b_) noexcept
			: m_a(a_)
			, m_b(b_)
		{
			reup(a_, b_);
		}

		bool valid() const noexcept;
		bool equal(const Rectangle2& _that) const noexcept;

		bool operator == (const Rectangle2& _that) const noexcept { return equal(_that); }

		Vector2f getA() const noexcept { return m_a; }
		Vector2f getB() const noexcept { return m_b; }

		double getDX() const noexcept { return m_b.x - m_a.x; }
		double getDY() const noexcept { return m_b.y - m_a.y; }

		void reup(const Vector2f& _a, const Vector2f& _b) noexcept;

		std::string dump() const;

		friend std::ostream& operator << (std::ostream& _stream, const Rectangle2& _sq) {
			return _stream << _sq.dump();
		}
	};

}//namespace la