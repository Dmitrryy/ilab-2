#include <fstream>
#include <sstream>

#include "include/Line.h"

#include "include/LALmath.h"

namespace la {

    Line2::Line2(la::Vector2f _p, la::Vector2f _v, Type _t /*= Type::TwoPoints*/) noexcept
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _p - _v;
        }
    }

    bool Line2::contein(const Vector2f& _rhs) const noexcept 
    { 
        if (valid()) {
            return (m_p - _rhs).collinear(m_v);
        }
        return false;
    }


    bool Line2::valid() const noexcept
    {
        return m_p.valid() && m_v.valid() && !m_v.isZero();
    }

    bool Line2::intersec(const Line2& _rhs) const noexcept
    {
        bool res = valid() && _rhs.valid();

        //parallel or same
        if (m_v.collinear(_rhs.getV()))
        {
            //same
            if (!(m_p - _rhs.m_p).collinear(m_v)) {
                res = false;
            }
        }

        return res;
    }

    std::string Line2::dump() const
    {
        std::ostringstream out;
        out << '[' << m_p << ", " << m_p + m_v << ']';

        return out.str();
    }


    Line3::Line3(la::Vector3f _p, la::Vector3f _v, Type _t /*= Type::TwoPoints*/) noexcept
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _p - _v;
        }
    }

    bool Line3::valid() const noexcept
    {
        return m_p.valid() && m_v.valid() && !m_v.isZero();
    }

    bool Line3::intersec(const Line3& _rhs) const
    {
        const auto res_find = findIntersec(*this, _rhs);

        bool res = false;
        res = res || res_find.second == Intersec::quantity::One;
        res = res || res_find.second == Intersec::quantity::Same;

        return res;
    }

    std::string Line3::dump() const
    {
        std::ostringstream out;
        out << '[' << m_p << ", " << m_p + m_v << ']';

        return out.str();
    }

}//namespace la