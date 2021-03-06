#pragma once

#include "string"

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

        Line2() noexcept
            : m_p(0)
            , m_v(0)
        {}

        Line2(la::Vector2f _p, la::Vector2f _v, Type _t = Type::TwoPoints) noexcept;

        bool valid() const noexcept;

        Vector2f getP () const noexcept { return m_p; }
        Vector2f getV () const noexcept { return m_v; }

        void reup     (la::Vector2f _a, la::Vector2f _b, Type _t = Type::TwoPoints) noexcept { *this = Line2(_a, _b, _t); }

        bool intersec (const Line2& _rhs) const noexcept;

        bool contein(const Vector2f& _rhs) const noexcept;

        std::string dump  () const;
        
        friend std::ostream& operator << (std::ostream& _stream, const Line2& _goal) {
            return _stream << _goal.dump();
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

        Line3() = default;

        Line3(la::Vector3f _p, la::Vector3f _v, Type _t = Type::TwoPoints) noexcept;

        bool valid() const noexcept;

        la::Vector3f getP() const noexcept { return m_p; }
        la::Vector3f getV() const noexcept { return m_v; }

        void reup    (la::Vector3f _a, la::Vector3f _b, Type _t = Type::TwoPoints) noexcept { *this = Line3(_a, _b, _t); }

        bool equal   (const Line3& _that) const noexcept { return contein(_that.getP()) && contein(_that.getP() + _that.getV()); }

        bool intersec(const Line3& _rhs) const;
        bool contein (const Vector3f& _rhs) const noexcept { return (m_p - _rhs).collinear(m_v); }

        bool operator == (const Line3& _that) const noexcept { return equal(_that); }

        std::string dump() const;

        friend std::ostream& operator << (std::ostream& _stream, const Line3& _goal) {
            return _stream << _goal.dump();
        }

    };//Line3

}//namespace la