#include "include/Vector.h"
#include "include/Line.h"
#include "include/LALmath.h"

#include <sstream>

namespace la {

    bool Vector3f::collinear(const la::Vector3f& _rhs) const noexcept {
        return product(*this, _rhs).isZero();
    }

    bool Vector3f::equal(const la::Vector3f& _rhs) const noexcept
    {
        bool result = true;
        result &= std::abs(x - _rhs.x) < EPSILON;
        result &= std::abs(y - _rhs.y) < EPSILON;
        result &= std::abs(z - _rhs.z) < EPSILON;

        return result;
    }

    std::string Vector2f::dump() const
    {
        std::ostringstream out;
        out << '(' << x << ", " << y << ')';
        return out.str();
    }

    Vector3f& Vector3f::operator += (const la::Vector3f& _rhs) noexcept
    {
        x += _rhs.x;
        y += _rhs.y;
        z += _rhs.z;

        return *this;
    }
    Vector3f& Vector3f::operator -= (const la::Vector3f& _rhs) noexcept
    {
        return *this = *this - _rhs;
    }

    Vector3f operator -  (const Vector3f& _lhs, const Vector3f& _rhs) noexcept
    {
        return Vector3f(_lhs.x - _rhs.x, _lhs.y - _rhs.y, _lhs.z - _rhs.z);
    }
    Vector3f operator +  (const Vector3f& _lhs, const Vector3f& _rhs) noexcept
    {
        return Vector3f(_lhs) += _rhs;
    }
    Vector3f operator *  (const Vector3f& _lhs, double _n) noexcept
    {
        return { _lhs.x * _n, _lhs.y * _n, _lhs.z * _n };
    }


    //////////////////////////////////////////////////////////////////////


    Vector2f& Vector2f::operator += (const la::Vector2f& _rhs) noexcept
    {
        x += _rhs.x;
        y += _rhs.y;

        return *this;
    }

    bool Vector2f::collinear(const la::Vector2f& _rhs) const noexcept
    {
        return product(*this, _rhs).isZero();
    }


    bool Vector2f::equal(const la::Vector2f& _rhs) const noexcept
    {
        bool result = true;
        result &= std::abs(x - _rhs.x) < EPSILON;
        result &= std::abs(y - _rhs.y) < EPSILON;

        return result;
    }

    std::string Vector3f::dump() const
    {
        std::ostringstream out;
        out << '(' << x << ", " << y << ", " << z << ')';
        return out.str();
    }

    Vector2f operator -  (const Vector2f& _lhs, const Vector2f& _rhs) noexcept
    {
        return Vector2f(_lhs.x - _rhs.x, _lhs.y - _rhs.y);
    }
    Vector2f operator +  (const Vector2f& _lhs, const Vector2f& _rhs) noexcept
    {
        Vector2f tmp(_lhs);
        return tmp += _rhs;
    }
    Vector2f operator *  (const Vector2f& _lhs, double _n) noexcept
    {
        return { _lhs.x * _n, _lhs.y * _n };
    }

}//namespace ezg