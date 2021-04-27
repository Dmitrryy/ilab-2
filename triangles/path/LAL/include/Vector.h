#pragma once

#include "config.h"

#include <iomanip>

namespace la {

    struct Vector2f
    {
        double x;
        double y;

        Vector2f() noexcept
            : x(0)
            , y(0)
        {};

        Vector2f(double _x, double _y) noexcept
            : x(_x)
            , y(_y)
        {};

        explicit Vector2f(double _all) noexcept
            : x(_all)
            , y(_all)
        {};

        double modul() const { return std::sqrt(x * x + y * y); }

        bool   valid() const noexcept;

        bool      operator == (const la::Vector2f& _rhs) const noexcept { return equal(_rhs); }
        Vector2f& operator += (const la::Vector2f& _rhs) noexcept;

        bool collinear (const la::Vector2f& _rhs) const noexcept;
        bool equal     (const la::Vector2f& _rhs) const noexcept;
        bool isZero    () const noexcept { return equal(Vector2f(0.0)); }

        static Vector2f make_invalid() noexcept { return Vector2f(NAN); }

        std::string dump() const;

        friend std::ostream& operator << (std::ostream& _stream, const Vector2f& _vec) {
            return _stream << _vec.dump();
        }
    };

    Vector2f operator -  (const Vector2f& _lhs, const Vector2f& _rhs) noexcept;
    Vector2f operator +  (const Vector2f& _lhs, const Vector2f& _rhs) noexcept;
    Vector2f operator *  (const Vector2f& _lhs, double _n) noexcept;
    inline Vector2f operator /  (const Vector2f& _lhs, double _n) noexcept { return _lhs * (1.0 / _n); };
    inline Vector2f operator *  (double _n, const Vector2f& _lhs) noexcept { return _lhs * _n; }

  
    ////////////////////////////////////////////////////////
    

    struct Vector3f
    {
        double x;
        double y;
        double z;

        Vector3f() noexcept
            : x(0)
            , y(0)
            , z(0)
        {}

        Vector3f(double _x, double _y, double _z) noexcept
            : x(_x)
            , y(_y)
            , z(_z)
        {};

        Vector3f(Vector2f _xy, double _z) noexcept
            : x(_xy.x)
            , y(_xy.y)
            , z(_z)
        {}

        explicit Vector3f(double _all) noexcept
            : x(_all)
            , y(_all)
            , z(_all)
        {};

        double modul() const { return std::sqrt(x * x + y * y + z * z); }

        bool   valid() const noexcept;

        bool       operator == (const la::Vector3f& _rhs) const noexcept { return equal(_rhs); }
        Vector3f&  operator += (const la::Vector3f& _rhs) noexcept;
        Vector3f&  operator -= (const la::Vector3f& _rhs) noexcept;

        bool collinear (const la::Vector3f& _rhs) const noexcept;
        bool equal     (const la::Vector3f& _rhs) const noexcept;
        bool isZero    () const noexcept { return equal(Vector3f(0)); }

        static Vector3f make_invalid() { return Vector3f(NAN); }

        std::string dump() const;

        friend std::ostream& operator << (std::ostream& _stream, const Vector3f& _vec) {
            return _stream << _vec.dump();
        }

    };

    Vector3f operator -  (const Vector3f& _lhs, const Vector3f& _rhs) noexcept;
    Vector3f operator +  (const Vector3f& _lhs, const Vector3f& _rhs) noexcept;
    Vector3f operator *  (const Vector3f& _lhs, double _n) noexcept;
    inline Vector3f operator /  (const Vector3f& _lhs, double _n) noexcept { return _lhs * (1 / _n); };
    inline Vector3f operator *  (double _n, const Vector3f& _lhs) noexcept { return _lhs * _n; }

}//namespace la