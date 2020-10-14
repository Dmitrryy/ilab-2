#pragma once

#include "config.h"

#include "Vector.h"
#include "Line.h"

namespace la {

    class LineSegment1
    {
        double m_a;
        double m_b;

    public:

        LineSegment1() noexcept
            : m_a(0.f)
            , m_b(0.f)
        {}

        LineSegment1(double _a, double _b) noexcept
        {
            m_a = std::min(_a, _b);
            m_b = std::max(_a, _b);
        }

        double length() const noexcept { return m_b - m_a; }

        bool   valid () const noexcept;

        double getA  ()   const noexcept { return m_a; }
        double getB  ()   const noexcept { return m_b; }

        bool equal   (const LineSegment1& _rhs) const noexcept;

        void reup    (double _a, double _b) noexcept { *this = LineSegment1(_a, _b); }

        bool operator == (const LineSegment1& _rhs) const noexcept { return equal(_rhs); }

        bool intersec(const LineSegment1& _rhs) const noexcept;
        bool contein (double _x) const noexcept { return _x >= m_a - EPSILON && _x <= m_b + EPSILON; }

        std::string dump () const;

        friend std::ostream& operator<<(std::ostream& _stream, const LineSegment1& _targ) {
            return _stream << _targ.dump();
        }
    };


    class LineSegment2
    {
        la::Vector2f m_p;
        la::Vector2f m_v;

    public:

        enum class Type {
            TwoPoints
            , PointAndVector
        };

        LineSegment2() = default;

        LineSegment2 (la::Vector2f _p, la::Vector2f _v, Type _t = Type::TwoPoints) noexcept;

        double length() const noexcept { return m_v.modul(); }

        bool equal   (const LineSegment2& _rhs) const noexcept { return m_p == _rhs.m_p && m_v == _rhs.m_v; }

        Vector2f getP() const noexcept { return m_p; }
        Vector2f getV() const noexcept { return m_v; }

        Line2 toLine () const noexcept { return { m_p, m_v, Line2::Type::PointAndVector }; }
        void reup    (la::Vector2f _a, la::Vector2f _b, Type _t = Type::TwoPoints) noexcept { *this = LineSegment2(_a, _b, _t); }

        bool operator == (const LineSegment2& _rhs) const noexcept { return equal(_rhs); }

        bool contein (const Vector2f& _rhs) const noexcept;

        std::string dump() const;

        friend std::ostream& operator<<(std::ostream& _stream, const LineSegment2& _targ) {
            return _stream << _targ.dump();
        }
    };


    class LineSegment3
    {
        la::Vector3f m_p;
        la::Vector3f m_v;

    public:

        enum class Type {
            TwoPoints
            , PointAndVector
        };

        LineSegment3() = default;

        LineSegment3(la::Vector3f _p, la::Vector3f _v, Type _t = Type::TwoPoints) noexcept;

        double length() const noexcept { return m_v.modul(); }

        bool   equal (const LineSegment3& _rhs) const noexcept { return m_p == _rhs.m_p && m_v == _rhs.m_v; }

        Vector3f getP() const noexcept { return m_p; }
        Vector3f getV() const noexcept { return m_v; }

        Line3 toLine () const noexcept { return { m_p, m_v, Line3::Type::PointAndVector }; }
        void  reup   (la::Vector3f _a, la::Vector3f _b, Type _t = Type::TwoPoints) noexcept { *this = LineSegment3(_a, _b, _t); }

        bool operator == (const LineSegment3& _rhs) const noexcept { return equal(_rhs); }

        bool contein (const Vector3f& _rhs) const noexcept;

        std::string dump() const;

        friend std::ostream& operator<<(std::ostream& _stream, const LineSegment3& _targ) {
            return _stream << _targ.dump();
        }
    };

}//namespace ezg