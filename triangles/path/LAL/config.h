#pragma once

#include <cmath>
#include <iostream>

namespace ezg {
	static const float EPSILON = 0.00001f;

    namespace Intersec {

        enum class quantity {
            Nop
            , One
            , Interval
            , Same
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