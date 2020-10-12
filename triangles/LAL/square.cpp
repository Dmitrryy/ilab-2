#include "include/square.h"

namespace la
{

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

}//namespace la