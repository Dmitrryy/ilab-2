#include "include/Vector.h"

#include "include/Line.h"

#include "include/LALmath.h"

namespace la {

    bool Vector3f::collinear(const la::Vector3f& _rhs) const noexcept
    {
        return crossProduct(*this, _rhs).isZero();
    }

    bool Vector3f::equal(const la::Vector3f& _rhs) const noexcept
    {
        bool result = true;
        result &= std::abs(x - _rhs.x) < EPSILON;
        result &= std::abs(y - _rhs.y) < EPSILON;
        result &= std::abs(z - _rhs.z) < EPSILON;

        return result;
    }

    //////////////////////////////////////////////////////////////////////


    bool Vector2f::collinear(const la::Vector2f& _rhs) const noexcept
    {
        return crossProduct(*this, _rhs).isZero();
    }


    bool Vector2f::equal(const la::Vector2f& _rhs) const noexcept
    {
        bool result = true;
        result &= std::abs(x - _rhs.x) < EPSILON;
        result &= std::abs(y - _rhs.y) < EPSILON;

        return result;
    }

}//namespace ezg