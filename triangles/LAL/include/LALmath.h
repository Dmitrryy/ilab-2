#pragma once


#include <cmath>

#include "Vector.h"
#include "Line.h"
#include "LineSegment.h"
#include "Plane.h"
#include "triangle.h"
#include "square.h"

///////////////////////////////////////////////////////////////
//
// Vector & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    Vector3f    product    (const Vector3f& _lhs, const Vector3f& _rhs) noexcept;
    double      dot        (const Vector3f& _lhs, const Vector3f& _rhs) noexcept;

    inline bool equal      (const Vector3f& _lhs, const Vector3f& _rhs) noexcept { return _lhs.equal(_rhs); }

    inline bool collinear  (const Vector3f& _lhs, const Vector3f& _rhs) noexcept { return _lhs.collinear(_rhs); }
    bool        codirected (const Vector3f& _lhs, const Vector3f& _rhs) noexcept;

    Vector3f    normalization (const Vector3f& _that) noexcept;

    double      modul      (const Vector3f& _v) noexcept;


//2D
    Vector3f    product    (const Vector2f& _lhs, const Vector2f& _rhs) noexcept;
    double      dot        (const Vector2f& _lhs, const Vector2f& _rhs) noexcept;

    inline bool equal      (const Vector2f& _lhs, const Vector2f& _rhs) noexcept { return _lhs.equal(_rhs); }
    
    inline bool collinear  (const Vector2f& _lhs, const Vector2f& _rhs) noexcept { return _lhs.collinear(_rhs); }
    
    Vector2f    normalization (const Vector2f& _that) noexcept;

    double      modul      (const Vector2f& _v) noexcept;

}//namespace la (Vector & VEctor)


///////////////////////////////////////////////////////////////
//
// Vector & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double        distance   (const Vector3f& _point, const Line3& _line) noexcept;
    Vector3f      projection (const Vector3f& _point, const Line3& _line) noexcept;

    inline double distance   (const Line3& _line, const Vector3f& _point) noexcept { return distance(_point, _line); }


//2D
    double        distance   (const Vector2f& _point, const Line2& _line) noexcept;
    Vector2f      projection (const Vector2f& _point, const Line2& _line) noexcept;

    inline double distance   (const Line2& _line, const Vector2f& _point) noexcept { return distance(_point, _line); }

}//namespace la (Vector & Line)


///////////////////////////////////////////////////////////////
//
// Line & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double distance      (const Line3& _lhs, const Line3& _rhs) noexcept;

    std::pair<Vector3f, Intersec::quantity> 
        findIntersec (const Line3& _lhs, const Line3& _rhs) noexcept;

//2D
    std::pair<Vector2f, Intersec::quantity> 
        findIntersec (const Line2& _lhs, const Line2& _rhs) noexcept;

}//namespace la (Line& Line)


///////////////////////////////////////////////////////////////
//
// LineSegment & LineSegment
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    std::pair<LineSegment3, Intersec::quantity>
        findIntersec (const LineSegment3& _lhs, const LineSegment3& _rhs);

//2D
    std::pair<LineSegment2, Intersec::quantity> 
        findIntersec (const LineSegment2& _lhs, const LineSegment2& _rhs);

//1D
    std::pair<LineSegment1, Intersec::quantity>
        findIntersec (const LineSegment1& _lhs, const LineSegment1& _rhs);

    inline bool intersec (const LineSegment1& _lhs, const LineSegment1& _rhs) noexcept
    { return _lhs.intersec(_rhs); }

}//namespace la (LineSegment & LineSegment)


///////////////////////////////////////////////////////////////
//
// LineSegment & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    std::pair<Vector3f, Intersec::quantity>
        findIntersec (const LineSegment3& _ls, const Line3& _line);

    inline std::pair<Vector3f, Intersec::quantity>
        findIntersec (const Line3& _line, const LineSegment3& _ls) { return findIntersec(_ls, _line); }
}

///////////////////////////////////////////////////////////////
//
// Plane & Plane
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool   intersec         (const Plane& _lhs, const Plane& _rhs);

    double distanceWithSign (const Plane& _lhs, const Plane& _rhs);

    double distance         (const Plane& _lhs, const Plane& _rhs);

    bool   equal            (const Plane& _lhs, const Plane& _rhs) noexcept;

    std::pair<Line3, Intersec::quantity> 
        findIntersec(const Plane& _lhs, const Plane& _rhs);

}//namespace la (Plane & Plane)


///////////////////////////////////////////////////////////////
//
// Plane & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double        distance (const Plane& _pl, const Vector3f _point);
    inline double distance (const Vector3f _point, const Plane& _pl) { return distance(_pl, _point); }

    double        distanceWithSign (const Plane& _pl, const Vector3f _point);
    inline double distanceWithSign (const Vector3f _point, const Plane& _pl) { return distanceWithSign(_pl, _point); }

    Vector3f projection (const Vector3f& _point, const Plane& _pl);

    bool contein (const Plane& _pl, const Vector3f& _point) noexcept;

}//namespace la (Plane & Vector)


///////////////////////////////////////////////////////////////
//
// Plane & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool        intersec (const Plane& _pl, const Line3& _ln);
    inline bool intersec (const Line3& _ln, const Plane& _pl) { return intersec(_pl, _ln); }

    double        distanceWithSign (const Plane& _pl, const Line3 _ln);
    inline double distanceWithSign (const Line3& _ln, const Plane& _pl) { return distanceWithSign(_pl, _ln); }

    double        distance (const Plane& _pl, const Line3 _ln);
    inline double distance (const Line3 _ln, const Plane& _pl) { return distance(_pl, _ln); }

    Line3 projection (const Line3& _line, const Plane& _pl);

    std::pair<Vector3f, Intersec::quantity> 
        findIntersec (const Plane& _pl, const Line3& _ln);
    inline std::pair<Vector3f, Intersec::quantity> 
        findIntersec (const Line3& _ln, const Plane& _pl) { return findIntersec(_pl, _ln); }

    bool contein (const Plane& _pl, const Line3& _ln) noexcept;

}//namespace la (Plane & Line)


///////////////////////////////////////////////////////////////
//
// Triangle & Triangle
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool intersec (const Triangle& _lhs, const Triangle& _rhs);

}//namespace la (Triangle & Triangle)


///////////////////////////////////////////////////////////////
//
// Triangle & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
    //3D
    std::pair<LineSegment3, Intersec::quantity> 
        findIntersec (const Triangle& _tr, const Line3& _line);

    inline std::pair<LineSegment3, Intersec::quantity> 
        findIntersec (const Line3& _line, const Triangle& _tr) { return findIntersec(_tr, _line); }

}//namespace la (Triangle & Line)


///////////////////////////////////////////////////////////////
//
// Square & Square
//
///////////////////////////////////////////////////////////////
namespace la
{
    //3D
    bool intersec (const Square& _lhs, const Square& _rhs);

    bool contein  (const Square& _lhs, const Square& _rhs);
    bool contein  (const Square& _sq, const Vector3f& _vec);

}//namespace la (Square & Square)