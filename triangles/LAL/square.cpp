#include <sstream>

#include "include/square.h"

namespace la
{

	bool Rectangle3::equal(const Rectangle3& _that) const noexcept
	{
		return m_a == _that.getA() && m_b == _that.getB();
	}

	void Rectangle3::reup(const Vector3f& _a, const Vector3f& _b) noexcept
	{
	    if (!_a.valid() || !_b.valid()) {
	        return ;
	    }
		m_a = _a;
		m_b = _b;
		if (_a.z <= _b.z) {
			if (_a.x <= _b.x && _a.y <= _b.y)
			{
				/*nop*/
			}
			else if (_a.x <= _b.x && _a.y >= _b.y)
			{
				const double dy = _a.y - _b.y;
				m_a.y -= dy;
				m_b.y += dy;
			}
			else if (_a.x >= _b.x && _a.y <= _b.y)
			{
				const double dx = _a.x - _b.x;
				m_a.x -= dx;
				m_b.x += dx;
			}
			else if (_a.x >= _b.x && _a.y >= _b.y)
			{
				const double dx = _a.x - _b.x;
				const double dy = _a.y - _b.y;

				m_a += Vector3f(-dx, -dy, 0);
				m_b += Vector3f(dx, dy, 0);
			}
		}
		else
		{
			reup(_b, _a);
		}
	}

	bool Rectangle3::valid() const noexcept
	{
		return m_a.valid() && m_b.valid() && !(m_a == m_b);
	}

	std::string Rectangle3::dump() const
	{
		std::ostringstream out;

		out << '[' << getA() << ", " << getB() << ']';
		return out.str();
	}

	///////////////////////////////////////////////////////////////////////////

	bool Rectangle2::valid() const noexcept
	{
		return m_a.valid() && m_b.valid() && !(m_a == m_b);
	}

	bool Rectangle2::equal(const Rectangle2& that_) const noexcept
	{
		return m_a == that_.getA() && m_b == that_.getB();
	}

	void Rectangle2::reup(const Vector2f& a_, const Vector2f& b_) noexcept
	{
		m_a = a_;
		m_b = b_;

		if (a_.x <= b_.x && a_.y <= b_.y)
		{
			/*nop*/
		}
		else if (a_.x <= b_.x && a_.y >= b_.y)
		{
			const double dy = a_.y - b_.y;
			m_a.y -= dy;
			m_b.y += dy;
		}
		else if (a_.x >= b_.x && a_.y <= b_.y)
		{
			const double dx = a_.x - b_.x;
			m_a.x -= dx;
			m_b.x += dx;
		}
		else if (a_.x >= b_.x && a_.y >= b_.y)
		{
			const double dx = a_.x - b_.x;
			const double dy = a_.y - b_.y;

			m_a += Vector2f(-dx, -dy);
			m_b += Vector2f(dx, dy);
		}
	}

	std::string Rectangle2::dump() const
	{
		std::ostringstream out;

		out << '[' << getA() << ", " << getB() << ']';
		return out.str();
	}

}//namespace la