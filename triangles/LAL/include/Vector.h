#pragma once

#include "config.h"

#include <iomanip>

namespace la {

    struct Vector2f
    {
    public:

        Vector2f()
            : x(0.f)
            , y(0.f)
        {};

        Vector2f(float _x, float _y)
            : x(_x)
            , y(_y)
        {};

        explicit Vector2f(float _all)
            : x(_all)
            , y(_all)
        {};

        float modul() const { return std::sqrt(x * x + y * y); }

        bool operator == (const la::Vector2f& _rhs) const noexcept { return equal(_rhs); }

        bool collinear(const la::Vector2f& _rhs) const noexcept;
        bool equal(const la::Vector2f& _rhs) const noexcept;
        bool isZero() const noexcept { return equal(Vector2f(0.f)); }

        friend Vector2f operator -  (const Vector2f& _lhs, const Vector2f& _rhs);
        friend Vector2f operator +  (const Vector2f& _lhs, const Vector2f& _rhs);

        friend std::ostream& operator << (std::ostream& _stream, const Vector2f& _vec)
        {
            _stream << '(' << _vec.x << "; " << _vec.y << ')';
            return _stream;
        }


        float x;
        float y;
    };

  
    /// ////////////////////////////////////////////////////
    

    struct Vector3f
    {
        float x;
        float y;
        float z;

    public:

        Vector3f() noexcept
            : x(0.f)
            , y(0.f)
            , z(0.f)
        {}

        Vector3f(float _x, float _y, float _z) noexcept
            : x(_x)
            , y(_y)
            , z(_z)
        {};

        Vector3f(Vector2f _xy, float _z) noexcept
            : x(_xy.x)
            , y(_xy.y)
            , z(_z)
        {}

        explicit Vector3f(float _all) noexcept
            : x(_all)
            , y(_all)
            , z(_all)
        {};

    public:

        float modul() const { return std::sqrt(x * x + y * y + z * z); }

        bool  operator == (const la::Vector3f& _rhs) const noexcept { return equal(_rhs); }

        friend Vector3f operator -  (const Vector3f& _lhs, const Vector3f& _rhs);
        friend Vector3f operator +  (const Vector3f& _lhs, const Vector3f& _rhs);

        bool collinear(const la::Vector3f& _rhs) const noexcept;
        bool equal(const la::Vector3f& _rhs) const noexcept;
        bool isZero() const noexcept { return equal(Vector3f(0.f)); }

        friend std::ostream& operator << (std::ostream& _stream, const Vector3f& _vec)
        {
            _stream << '(' << _vec.x << "; " << _vec.y << "; " << _vec.z << ')';
            return _stream;
        }

    };

}//namespace ezg