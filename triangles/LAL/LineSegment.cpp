#include "include/LineSegment.h"

#include <cassert>

#include "include/LALmath.h"

namespace la 
{
    /////////////////////////////LineSegment1//////////////////////////////////

    bool LineSegment1::intersection(const LineSegment1& _rhs) const noexcept
    {
        bool res = false;

        res = res || contein(_rhs.m_a);
        res = res || contein(_rhs.m_b);
        res = res || _rhs.contein(m_a);
        res = res || _rhs.contein(m_b);

        return res;
    }


    /////////////////////////////LineSegment2//////////////////////////////////

    LineSegment2::LineSegment2(la::Vector2f _p, la::Vector2f _v, Type _t /*= Type::TwoPoints*/)
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _v - _p;
        }
    }



    bool LineSegment2::contein(const Vector2f& _rhs) const
    {
        bool res = false;
        if (toLine().contein(_rhs))
        {
            float s = scalarProduct(normalization(m_v), _rhs - m_p) / m_v.modul();
            if (s >= 0.f && s <= 1.f) {
                res = true;
            }
        }
        return res;
    }

    /////////////////////////////LineSegment3//////////////////////////////////

    LineSegment3::LineSegment3(la::Vector3f _p, la::Vector3f _v, Type _t /*= Type::TwoPoints*/)
        : m_p(_p)
        , m_v(_v)
    {
        if (_t == Type::TwoPoints) {
            m_v = _v - _p;
        }
    }

    bool LineSegment3::contein(const Vector3f& _rhs) const
    {
        bool res = false;
        if (toLine().contein(_rhs))
        {
            float s = scalarProduct(normalization(m_v), _rhs - m_p) / m_v.modul();
            if (s >= 0.f && s <= 1.f + EPSILON) {
                res = true;
            }
        }
        return res;
    }

}//namespae la