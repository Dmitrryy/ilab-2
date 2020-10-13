#include "include/square.h"

namespace la
{

	bool Square::equal(const Square& _that) const noexcept
	{
		return m_a == _that.getA() && m_b == _that.getB();
	}

	void Square::reup(const Vector3f& _a, const Vector3f& _b) noexcept
	{
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

	std::ostream& operator << (std::ostream& _stream, const Square& _sq)
	{
		_stream << '[' << _sq.getA() << ", " << _sq.getB() << ']';
		return _stream;
	}

}//namespace la