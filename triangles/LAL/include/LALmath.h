#pragma once


#include <cmath>

#include "Vector.h"
#include "Line.h"
#include "LineSegment.h"
#include "Plane.h"

///////////////////////////////////////////////////////////////
//
// Vector & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    Vector3f  crossProduct(const Vector3f& _lhs, const Vector3f& _rhs);
    float     scalarProduct(const Vector3f& _lhs, const Vector3f& _rhs);

    inline bool   equal(const Vector3f& _lhs, const Vector3f& _rhs) { return _lhs.equal(_rhs); }

    inline bool   collinear(const Vector3f& _lhs, const Vector3f& _rhs) { return _lhs.collinear(_rhs); }

    Vector3f  normalization(const Vector3f& _that);

    float modul(const Vector3f& _v);

    Vector3f operator -  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator +  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator *  (const Vector3f& _lhs, const Vector3f& _rhs);
    Vector3f operator *  (const Vector3f& _lhs, float _n);
    inline Vector3f operator *  (float _n, const Vector3f& _lhs) { return _lhs * _n; }
    float    operator ^  (const Vector3f& _lhs, const Vector3f& _rhs);


//2D
    Vector3f crossProduct(const Vector2f& _lhs, const Vector2f& _rhs);
    float    scalarProduct(const Vector2f& _lhs, const Vector2f& _rhs);

    inline bool  equal(const Vector2f& _lhs, const Vector2f& _rhs) { return _lhs.equal(_rhs); }
    
    inline bool  collinear(const Vector2f& _lhs, const Vector2f& _rhs) { return _lhs.collinear(_rhs); }
    
    Vector2f normalization(const Vector2f& _that);

    float modul(const Vector2f& _v);

    Vector2f operator -  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector2f operator +  (const Vector2f& _lhs, const Vector2f& _rhs);
    float        operator ^  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector3f operator *  (const Vector2f& _lhs, const Vector2f& _rhs);
    Vector2f operator *  (const Vector2f& _lhs, float _n);
    inline Vector2f operator *  (float _n, const Vector2f& _lhs) { return _lhs * _n; }

}//namespace la (Vector & VEctor)


///////////////////////////////////////////////////////////////
//
// Vector & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    float    distance(const Vector3f& _point, const Line3& _line);
    Vector3f projection(const Vector3f& _point, const Line3& _line);

    inline float distance(const Line3& _line, const Vector3f& _point) { return distance(_point, _line); }


//2D
    float    distance(const Vector2f& _point, const Line2& _line);
    Vector2f projection(const Vector2f& _point, const Line2& _line);

    inline float distance(const Line2& _line, const Vector2f& _point) { return distance(_point, _line); }

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

    inline bool intersection(const LineSegment1& _lhs, const LineSegment1& _rhs) 
    { return _lhs.intersection(_rhs); }

}//namespace la (LineSegment & LineSegment)


///////////////////////////////////////////////////////////////
//
// Plane & Plane
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool intersec(const Plane& _lhs, const Plane& _rhs);

    float distanceWithSign(const Plane& _lhs, const Plane& _rhs);

    float distance(const Plane& _lhs, const Plane& _rhs);

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
    float distance(const Plane& _pl, const Vector3f _point);
    inline float distance(const Vector3f _point, const Plane& _pl) { return distance(_pl, _point); }

    float distanceWithSign(const Plane& _pl, const Vector3f _point);
    inline float distanceWithSign(const Vector3f _point, const Plane& _pl) { return distanceWithSign(_pl, _point); }

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

    float distanceWithSign(const Plane& _pl, const Line3 _ln);
    inline float distanceWithSign(const Line3& _ln, const Plane& _pl) { return distanceWithSign(_pl, _ln); }

    float distance(const Plane& _pl, const Line3 _ln);
    inline float distance(const Line3 _ln, const Plane& _pl) { return distance(_pl, _ln); }

    Line3 projection(const Line3& _line, const Plane& _pl);

    std::pair<Vector3f, Intersec::quantity> 
        findIntersec(const Plane& _pl, const Line3& _ln);
    inline std::pair<Vector3f, Intersec::quantity> 
        findIntersec(const Line3& _ln, const Plane& _pl) { return findIntersec(_pl, _ln); }

    bool contein(const Plane& _pl, const Line3& _ln);

}//namespace la (Plane & Line)