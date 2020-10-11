#pragma once


#include <cmath>

#include "Vector.h"
#include "Line.h"
#include "LineSegment.h"
#include "Plane.h"
#include "triangle.h"

///////////////////////////////////////////////////////////////
//
// Vector & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    Vector3f  crossProduct(const Vector3f& _lhs, const Vector3f& _rhs);
    double     scalarProduct(const Vector3f& _lhs, const Vector3f& _rhs);

    inline bool   equal(const Vector3f& _lhs, const Vector3f& _rhs) { return _lhs.equal(_rhs); }

    inline bool   collinear(const Vector3f& _lhs, const Vector3f& _rhs) { return _lhs.collinear(_rhs); }
    bool codirected(const Vector3f& _lhs, const Vector3f& _rhs);

    Vector3f  normalization(const Vector3f& _that);

    double modul(const Vector3f& _v);

    Vector3f operator -  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator +  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator *  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator *  (const Vector3f& _lhs, double _n);
    inline Vector3f operator /  (const Vector3f& _lhs, double _n) { return _lhs * (1 / _n); };
    inline Vector3f operator *  (double _n, const Vector3f& _lhs) { return _lhs * _n; }
    double    operator ^  (const Vector3f& _lhs, const Vector3f& _rhs);


//2D
    Vector3f crossProduct(const Vector2f& _lhs, const Vector2f& _rhs);
    double    scalarProduct(const Vector2f& _lhs, const Vector2f& _rhs);

    inline bool  equal(const Vector2f& _lhs, const Vector2f& _rhs) { return _lhs.equal(_rhs); }
    
    inline bool  collinear(const Vector2f& _lhs, const Vector2f& _rhs) { return _lhs.collinear(_rhs); }
    
    Vector2f normalization(const Vector2f& _that);

    double modul(const Vector2f& _v);

    Vector2f operator -  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector2f operator +  (const Vector2f& _lhs, const Vector2f& _rhs);
    double        operator ^  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector3f operator *  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector2f operator *  (const Vector2f& _lhs, double _n);
    inline Vector2f operator *  (double _n, const Vector2f& _lhs) { return _lhs * _n; }

}//namespace la (Vector & VEctor)


///////////////////////////////////////////////////////////////
//
// Vector & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double    distance(const Vector3f& _point, const Line3& _line);
    Vector3f projection(const Vector3f& _point, const Line3& _line);

    inline double distance(const Line3& _line, const Vector3f& _point) { return distance(_point, _line); }


//2D
    double    distance(const Vector2f& _point, const Line2& _line);
    Vector2f projection(const Vector2f& _point, const Line2& _line);

    inline double distance(const Line2& _line, const Vector2f& _point) { return distance(_point, _line); }

}//namespace la (Vector & Line)


///////////////////////////////////////////////////////////////
//
// Line & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double distance(const Line3& _lhs, const Line3& _rhs);
    std::pair<Vector3f, Intersec::quantity> findIntersection(const Line3& _lhs, const Line3& _rhs);

//2D
    std::pair<Vector2f, Intersec::quantity> findIntersection(const Line2& _lhs, const Line2& _rhs);

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
        findIntersection(const LineSegment3& _lhs, const LineSegment3& _rhs);

//2D
    std::pair<LineSegment2, Intersec::quantity> 
        findIntersection(const LineSegment2& _lhs, const LineSegment2& _rhs);

//1D
    std::pair<LineSegment1, Intersec::quantity>
        findIntersection(const LineSegment1& _lhs, const LineSegment1& _rhs);

    inline bool intersection(const LineSegment1& _lhs, const LineSegment1& _rhs) noexcept
    { return _lhs.intersection(_rhs); }

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
        findIntersection(const LineSegment3& _ls, const Line3& _line);

    inline std::pair<Vector3f, Intersec::quantity>
        findIntersection(const Line3& _line, const LineSegment3& _ls) { return findIntersection(_ls, _line); }
}

///////////////////////////////////////////////////////////////
//
// Plane & Plane
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool intersec(const Plane& _lhs, const Plane& _rhs);

    double distanceWithSign(const Plane& _lhs, const Plane& _rhs);

    double distance(const Plane& _lhs, const Plane& _rhs);

    bool equal(const Plane& _lhs, const Plane& _rhs);

    std::pair<Line3, Intersec::quantity> findIntersec(const Plane& _lhs, const Plane& _rhs);

}//namespace la (Plane & Plane)


///////////////////////////////////////////////////////////////
//
// Plane & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    double distance(const Plane& _pl, const Vector3f _point);
    inline double distance(const Vector3f _point, const Plane& _pl) { return distance(_pl, _point); }

    double distanceWithSign(const Plane& _pl, const Vector3f _point);
    inline double distanceWithSign(const Vector3f _point, const Plane& _pl) { return distanceWithSign(_pl, _point); }

    Vector3f projection(const Vector3f& _point, const Plane& _pl);

    bool contein(const Plane& _pl, const Vector3f& _point);

}//namespace la (Plane & Vector)


///////////////////////////////////////////////////////////////
//
// Plane & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool intersec(const Plane& _pl, const Line3& _ln);
    inline bool intersec(const Line3& _ln, const Plane& _pl) { return intersec(_pl, _ln); }

    double distanceWithSign(const Plane& _pl, const Line3 _ln);
    inline double distanceWithSign(const Line3& _ln, const Plane& _pl) { return distanceWithSign(_pl, _ln); }

    double distance(const Plane& _pl, const Line3 _ln);
    inline double distance(const Line3 _ln, const Plane& _pl) { return distance(_pl, _ln); }

    Line3 projection(const Line3& _line, const Plane& _pl);

    std::pair<Vector3f, Intersec::quantity> 
        findIntersec(const Plane& _pl, const Line3& _ln);
    inline std::pair<Vector3f, Intersec::quantity> 
        findIntersec(const Line3& _ln, const Plane& _pl) { return findIntersec(_pl, _ln); }

    bool contein(const Plane& _pl, const Line3& _ln);

}//namespace la (Plane & Line)


///////////////////////////////////////////////////////////////
//
// Triangle & Triangle
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool intersec(const Triangle& _lhs, const Triangle& _rhs);

}//namespace la (Triangle & Triangle)


///////////////////////////////////////////////////////////////
//
// Triangle & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
    //3D
    std::pair<LineSegment3, Intersec::quantity> findIntersec(const Triangle& _tr, const Line3& _line);
    inline std::pair<LineSegment3, Intersec::quantity> 
        findIntersec(const Line3& _line, const Triangle& _tr) { return findIntersec(_tr, _line); }

}//namespace la (Triangle & Line)