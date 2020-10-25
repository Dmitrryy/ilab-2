#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>

namespace la {
	static constexpr double EPSILON = 0.00001;

    namespace Intersec {

        enum class quantity {
            Nop
            , One
            , Interval
            , Same
            , Error
        };

    }

    class Vector2f;
    class Vector3f;

    class Line2;
    class Line3;

    class LineSegment1;
    class LineSegment2;
    class LineSegment3;

    class Plane;
}//namespace ezg