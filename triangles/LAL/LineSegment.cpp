#include "include/LineSegment.h"

#include <cassert>
#include <sstream>

#include "include/LALmath.h"

namespace la 
{
    /////////////////////////////LineSegment1//////////////////////////////////

    bool LineSegment1::intersec(const LineSegment1& _rhs) const noexcept
    {
        bool res = false;

        res = res || contein(_rhs.m_a);
        res = res || contein(_rhs.m_b);
        res = res || _rhs.contein(m_a);
        res = res || _rhs.contein(m_b);

        return res;
    }

    bool LineSegment1::valid() const noexcept
    {
        return (std::isnormal(m_a) && std::isnormal(m_b)) 
            || (m_a == 0.0 && std::isnormal(m_b))
            || (m_b == 0.0 && std::isnormal(m_a))
            || (m_a == 0.0 && m_b == 0.0);
    }

    bool LineSegment1::equal(const LineSegment1& _rhs) const noexcept {
        return std::abs(m_a - _rhs.m_a) < EPSILON && std::abs(m_b - _rhs.m_b) < EPSILON;
    }

    std::string LineSegment1::dump() const
    {
        std::ostringstream out;
        out << '[' << m_a << ", " << m_b << ']';

        return out.str();
    }

    /////////////////////////////LineSegment2//////////////////////////////////

    LineSegment2::LineSegment2(la::Vector2f _p, la::Vector2f _v, Type _t /*= Type::TwoPoints*/) noexcept
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _v - _p;
        }
    }

    bool LineSegment2::valid() const
    {
        return m_p.valid() && m_v.valid();
    }

    bool LineSegment2::contein(const Vector2f& _rhs) const noexcept
    {
        bool res = false;
        if (toLine().contein(_rhs))
        {
            const double s = dot(normalization(m_v), _rhs - m_p) / m_v.modul();
            if (s >= -EPSILON && s <= 1.f + EPSILON) {
                res = true;
            }
        }
        return res;
    }

    std::string LineSegment2::dump() const
    {
        std::ostringstream out;
        out << '[' << m_p << ", " << m_p + m_v << ']';

        return out.str();
    }

    /////////////////////////////LineSegment3//////////////////////////////////

    LineSegment3::LineSegment3(la::Vector3f _p, la::Vector3f _v, Type _t /*= Type::TwoPoints*/) noexcept
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _v - _p;
        }
    }

    bool LineSegment3::valid() const noexcept
    {
        return m_p.valid() && m_v.valid();
    }

    bool LineSegment3::contein(const Vector3f& _rhs) const noexcept
    {
        bool res = false;
        if (toLine().contein(_rhs))
        {
            const double s = dot(normalization(m_v), _rhs - m_p) / m_v.modul();
            if (s >= 0.f && s <= 1.f + EPSILON) {
                res = true;
            }
        }
        return res;
    }

    std::string LineSegment3::dump() const
    {
        std::ostringstream out;
        out << '[' << m_p << ", " << m_p + m_v << ']';

        return out.str();
    }

}//namespae la