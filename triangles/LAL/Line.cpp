
#include "include/Line.h"

#include "include/LALmath.h"


namespace la {

    Line2::Line2(la::Vector2f _p, la::Vector2f _v, Type _t /*= Type::TwoPoints*/)
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _p - _v;
        }
    }

    bool Line2::intersection(const Line2& _rhs) const
    {
        bool res = true;

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


    Line3::Line3(la::Vector3f _p, la::Vector3f _v, Type _t /*= Type::TwoPoints*/)
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _p - _v;
        }
    }

    bool Line3::intersection(const Line3& _rhs) const
    {
        auto res_find = findIntersection(*this, _rhs);

        bool res = false;
        res = res || res_find.second == Intersec::quantity::One;
        res = res || res_find.second == Intersec::quantity::Same;

        return res;
    }

}//namespace la