#pragma once

#include "config.h"
#include "Vector.h"

namespace la {

    class Line2
    {
        la::Vector2f m_p;
        la::Vector2f m_v;

    public:

        enum class Type {
            TwoPoints
            , PointAndVector
        };

        Line2()
            : m_p(0)
            , m_v(0)
        {}

        Line2(la::Vector2f _p, la::Vector2f _v, Type _t = Type::TwoPoints);

        Vector2f getP() const noexcept { return m_p; }
        Vector2f getV() const noexcept { return m_v; }

        void reup(la::Vector2f _a, la::Vector2f _b, Type _t = Type::TwoPoints) { *this = Line2(_a, _b, _t); }

        bool intersection(const Line2& _rhs) const;

        bool contein(const Vector2f& _rhs) const { return (m_p - _rhs).collinear(m_v); }
        
        friend std::ostream& operator << (std::ostream& _stream, const Line2& _goal) {
            _stream << '[' << _goal.m_p << ", " << _goal.m_p + _goal.m_v << ']';

            return _stream;
        }
    };//Line2


    class Line3
    {
        la::Vector3f m_p;
        la::Vector3f m_v;

    public:

        enum class Type {
            TwoPoints
            , PointAndVector
        };

        Line3()
            : m_p(0)
            , m_v(0)
        {}

        Line3(la::Vector3f _p, la::Vector3f _v, Type _t = Type::TwoPoints);

        la::Vector3f getP() const noexcept { return m_p; }
        la::Vector3f getV() const noexcept { return m_v; }

        void reup(la::Vector3f _a, la::Vector3f _b, Type _t = Type::TwoPoints) { *this = Line3(_a, _b, _t); }

        bool equal(const Line3& _that) const noexcept { return contein(_that.getP()) && contein(_that.getP() + _that.getV()); }

        bool intersection(const Line3& _rhs) const;
        bool contein(const Vector3f& _rhs) const { return (m_p - _rhs).collinear(m_v); }

        bool operator == (const Line3& _that) const noexcept { return equal(_that); }

        friend std::ostream& operator << (std::ostream& _stream, const Line3& _goal) {
            _stream << '[' << _goal.m_p << ", " << _goal.m_p + _goal.m_v << ']';

            return _stream;
        }
    };//Line3

}//namespace la