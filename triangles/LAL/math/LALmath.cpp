#include "../include/LALmath.h"


///////////////////////////////////////////////////////////////
//
// Vector & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
    la::Vector3f normalization(const la::Vector3f& _that)
    {
        if (!_that.isZero())
        {
            float m = _that.modul();
            return la::Vector3f(_that.x / m, _that.y / m, _that.z / m);
        }
        else
        {
            return la::Vector3f(0.f);
        }
    }

    float modul(const Vector3f& _v)
    {
        return std::sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z);
    }

    la::Vector3f crossProduct(const la::Vector3f& _lhs, const la::Vector3f& _rhs)
    {
        auto res = la::Vector3f(_lhs.y * _rhs.z - _lhs.z * _rhs.y,
            _lhs.z * _rhs.x - _lhs.x * _rhs.z,
            _lhs.x * _rhs.y - _lhs.y * _rhs.x);

        return res;
    }


    float scalarProduct(const la::Vector3f& _lhs, const la::Vector3f& _rhs)
    {
        return _lhs.x * _rhs.x + _lhs.y * _rhs.y + _lhs.z * _rhs.z;
    }


    la::Vector3f operator -  (const la::Vector3f& _lhs, const la::Vector3f& _rhs) 
    { 
        return la::Vector3f(_lhs.x - _rhs.x, _lhs.y - _rhs.y, _lhs.z - _rhs.z); 
    }
    la::Vector3f operator +  (const la::Vector3f& _lhs, const la::Vector3f& _rhs) 
    { 
        return la::Vector3f(_lhs.x + _rhs.x, _lhs.y + _rhs.y, _lhs.z + _rhs.z); 
    }
    la::Vector3f operator *  (const la::Vector3f& _lhs, const la::Vector3f& _rhs) 
    { 
        return crossProduct(_lhs, _rhs); 
    }
    la::Vector3f operator *  (const la::Vector3f& _lhs, float _n) 
    { 
        return { _lhs.x * _n, _lhs.y * _n, _lhs.z * _n }; 
    }
    float        operator ^  (const la::Vector3f& _lhs, const la::Vector3f& _rhs) 
    { 
        return scalarProduct(_lhs, _rhs); 
    }


//2D
    la::Vector2f normalization(const la::Vector2f& _that)
    {
        if (!_that.isZero())
        {
            float m = _that.modul();
            return la::Vector2f(_that.x / m, _that.y / m);
        }
        else
        {
            return la::Vector2f(0.f);
        }
    }

    float modul(const Vector2f& _v) 
    { 
        return std::sqrt(_v.x * _v.x + _v.y * _v.y); 
    }


    la::Vector3f crossProduct(const la::Vector2f& _lhs, const la::Vector2f& _rhs)
    {
        return crossProduct({ _lhs.x, _lhs.y, 0.f }, { _rhs.x, _rhs.y, 0.f });
    }

    float scalarProduct(const la::Vector2f& _lhs, const la::Vector2f& _rhs)
    {
        return (static_cast<double>(_lhs.x) * _rhs.x
            + static_cast<double>(_lhs.y) * _rhs.y);
    }


    la::Vector2f operator -  (const la::Vector2f& _lhs, const la::Vector2f& _rhs) 
    { 
        return la::Vector2f(_lhs.x - _rhs.x, _lhs.y - _rhs.y);
    }
    la::Vector2f operator +  (const la::Vector2f& _lhs, const la::Vector2f& _rhs) 
    { 
        return la::Vector2f(_lhs.x + _rhs.x, _lhs.y + _rhs.y);
    }
    float        operator ^  (const la::Vector2f& _lhs, const la::Vector2f& _rhs) 
    { 
        return scalarProduct(_lhs, _rhs);
    }
    la::Vector3f operator *  (const la::Vector2f& _lhs, const la::Vector2f& _rhs)
    { 
        return crossProduct(_lhs, _rhs);
    }
    la::Vector2f operator *  (const la::Vector2f& _lhs, float _n) 
    { 
        return { _lhs.x * _n, _lhs.y * _n };
    }

}//namespace la (Vector & Vector)



///////////////////////////////////////////////////////////////
//
// Vector & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    Vector3f projection(const Vector3f& _point, const Line3& _line)
    {
        Vector3f tmp_lv = normalization(_line.getV());

        float s = scalarProduct(tmp_lv, _point - _line.getP());

        return _line.getP() + tmp_lv * s;
    }


    float distance(const Vector3f& _point, const Line3& _line)
    {
        return (_point - projection(_point, _line)).modul();
    }

//2D
    Vector2f projection(const Vector2f& _point, const Line2& _line)
    {
        Vector2f tmp_lv = normalization(_line.getV());

        float s = scalarProduct(tmp_lv, _point - _line.getP());

        return _line.getP() + tmp_lv * s;
    }


    float distance(const Vector2f& _point, const Line2& _line)
    {
        return (_point - projection(_point, _line)).modul();
    }

}//namespace la (Vector & Line)



///////////////////////////////////////////////////////////////
//
// Line & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    std::pair<Vector3f, Intersec::quantity> findIntersection(const Line3& _lhs, const Line3& _rhs)
    {
        //Q1 and Q2 are points -> dot(Q2 - Q1, V1) = 0 & dot(Q2 - Q1, V2) = 0
        //Q1 = P1 + S1 * V1
        //Q2 = P2 + S2 * V2
        //
        //...
        //
        //S1 * dot(V1, V1) - S2 * dot(V2, V1) = dot(P2 - P1, V1)
        //S1 * dot(V1, V2) - S2 * dot(V2, V2) = dot(P2 - P1, V2)
        //<=>
        //S1 * a - S2 * b = c
        //S2 * b - S2 * d = f

        Vector3f pp = _rhs.getP() - _lhs.getP();
        float a = scalarProduct(_lhs.getV(), _lhs.getV());
        float b = scalarProduct(_lhs.getV(), _rhs.getV());
        float c = scalarProduct(pp, _lhs.getV());
        float d = scalarProduct(_rhs.getV(), _rhs.getV());
        float f = scalarProduct(pp, _rhs.getV());
        float det = -a * d + b * b;

        std::pair<Vector3f, Intersec::quantity> res(0.f, Intersec::quantity::Nop);
        if (std::abs(det) < EPSILON)
        {
            //V1 || V2

            //V1 == V2
            if ((_lhs.getP() - _rhs.getP()).collinear(_lhs.getV()))
            {
                res.second = Intersec::quantity::Same;
            }
        }
        else
        {
            float det1 = -d * c + b * f;
            float det2 = a * f - b * c;

            float S1 = det1 / det;
            float S2 = det2 / det;

            Vector3f Q1 = _lhs.getP() + _lhs.getV() * S1;
            Vector3f Q2 = _rhs.getP() + _rhs.getV() * S2;

            if (Q1 == Q2)
            {
                res.first = Q1;
                res.second = Intersec::quantity::One;
            }
        }

        return res;
    }

    double distance(const Line3& _lhs, const Line3& _rhs)
    {
        //Q1 and Q2 are points -> dot(Q2 - Q1, V1) = 0 & dot(Q2 - Q1, V2) = 0
        //Q1 = P1 + S1 * V1
        //Q2 = P2 + S2 * V2
        //
        //...
        //
        //S1 * dot(V1, V1) - S2 * dot(V2, V1) = dot(P2 - P1, V1)
        //S1 * dot(V1, V2) - S2 * dot(V2, V2) = dot(P2 - P1, V2)
        //<=>
        //S1 * a - S2 * b = c
        //S2 * b - S2 * d = f

        Vector3f pp = _rhs.getP() - _lhs.getP();
        float a = scalarProduct(_lhs.getV(), _lhs.getV());
        float b = scalarProduct(_lhs.getV(), _rhs.getV());
        float c = scalarProduct(pp, _lhs.getV());
        float d = scalarProduct(_rhs.getV(), _rhs.getV());
        float f = scalarProduct(pp, _rhs.getV());
        float det = -a * d + b * b;

        float res = 0.f;
        if (std::abs(det) < EPSILON)
        {
            //V1 || V2

            //V1 == V2
            if ((_lhs.getP() - _rhs.getP()).collinear(_lhs.getV()))
            {
                res = 0.f;
            }
            else
            {
                float S2 = 0.f;
                float S1 = c / a;

                res = (_lhs.getP() + _lhs.getV() * S1 - _rhs.getP()).modul();
            }
        }
        else
        {
            float det1 = -d * c + b * f;
            float det2 = a * f - b * c;

            float S1 = det1 / det;
            float S2 = det2 / det;

            Vector3f Q1 = _lhs.getP() + _lhs.getV() * S1;
            Vector3f Q2 = _rhs.getP() + _rhs.getV() * S2;

            res = (Q1 - Q2).modul();
        }

        return res;
    }

//2D
    std::pair<Vector2f, Intersec::quantity> findIntersection(const Line2& _lhs, const Line2& _rhs)
    {
        if (!_lhs.intersection(_rhs)) {
            return { {}, Intersec::quantity::Nop };
        }

        std::pair<Vector2f, Intersec::quantity> res({}, Intersec::quantity::Nop);

        if (!_lhs.getV().collinear(_rhs.getV())) {
            float t = crossProduct(_rhs.getP() - _lhs.getP(), _rhs.getV()).z
                / crossProduct(_lhs.getV(), _rhs.getV()).z;
            res.first = _lhs.getP() + t * _lhs.getV();
            res.second = Intersec::quantity::One;
        }
        else {
            res.first = _lhs.getP();
            res.second = Intersec::quantity::Same;
        }

        return res;
    }

}//namespace la (Line & Line)



///////////////////////////////////////////////////////////////
//
// LineSegment & LineSegment
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    std::pair<LineSegment3, Intersec::quantity>
        findIntersection(const LineSegment3& _lhs, const LineSegment3& _rhs)
    {
        std::pair<Vector3f, Intersec::quantity> intersec_line = findIntersection(_lhs.toLine(), _rhs.toLine());

        std::pair<LineSegment3, Intersec::quantity> res({}, Intersec::quantity::Nop);
        if (intersec_line.second == Intersec::quantity::Nop) {
            /*nop*/
        }
        else if (intersec_line.second == Intersec::quantity::One)
        {
            if (_lhs.contein(intersec_line.first) && _rhs.contein(intersec_line.first))
            {
                res.first = LineSegment3(intersec_line.first, intersec_line.first, LineSegment3::Type::TwoPoints);
                res.second = Intersec::quantity::One;
            }
        }
        else if (intersec_line.second == Intersec::quantity::Same)
        {
            float s0 = scalarProduct(normalization(_lhs.getV()), _rhs.getP() - _lhs.getP()) / _lhs.getV().modul();
            float s1 = scalarProduct(normalization(_lhs.getV()), _rhs.getP() + _rhs.getV() - _lhs.getP()) / _lhs.getV().modul();

            auto inter_res = findIntersection({ 0.f, 1.f }, { s0, s1 });
            if (inter_res.second != Intersec::quantity::Nop)
            {
                res.second = inter_res.second;

                res.first = LineSegment3(_lhs.getP() + _lhs.getV() * inter_res.first.getA()
                    , _lhs.getP() + _lhs.getV() * inter_res.first.getB());
            }
        }
        return res;
    }


//2D
    std::pair<LineSegment2, Intersec::quantity>
        findIntersection(const LineSegment2& _lhs, const LineSegment2& _rhs)
    {
        std::pair<Vector2f, Intersec::quantity> intersec_line = findIntersection(_lhs.toLine(), _rhs.toLine());

        std::pair<LineSegment2, Intersec::quantity> res({}, Intersec::quantity::Nop);
        if (intersec_line.second == Intersec::quantity::Nop) {
            /*nop*/
        }
        else if (intersec_line.second == Intersec::quantity::One)
        {
            if (_lhs.contein(intersec_line.first) && _rhs.contein(intersec_line.first))
            {
                res.first = LineSegment2(intersec_line.first, intersec_line.first, LineSegment2::Type::TwoPoints);
                res.second = Intersec::quantity::One;
            }
        }
        else if (intersec_line.second == Intersec::quantity::Same)
        {
            float s0 = scalarProduct(normalization(_lhs.getV()), _rhs.getP() - _lhs.getP()) / _lhs.getV().modul();
            float s1 = scalarProduct(normalization(_lhs.getV()), _rhs.getP() + _rhs.getV() - _lhs.getP()) / _lhs.getV().modul();

            auto inter_res = findIntersection({ 0.f, 1.f }, { s0, s1 });
            if (inter_res.second != Intersec::quantity::Nop)
            {
                res.second = inter_res.second;

                res.first = LineSegment2(_lhs.getP() + _lhs.getV() * inter_res.first.getA(), _lhs.getP() + _lhs.getV() * inter_res.first.getB());
            }
        }
        return res;
    }

//1D
    std::pair<LineSegment1, Intersec::quantity>
        findIntersection(const LineSegment1& _lhs, const LineSegment1& _rhs)
    {
        auto res = std::make_pair(LineSegment1(), Intersec::quantity::Nop);

        if (!_lhs.intersection(_rhs)) {
            return res;
        }

        float res_a = 0.f;
        float res_b = 0.f;
        // m_a === a1
        // m_b === b1
        // ...
        //      a2*-----------*b2
        //  a1*--------*b1
        if (_lhs.getB() > _rhs.getA())
        {
            if (_lhs.getA() < _rhs.getB()) {
                if (_lhs.getA() < _rhs.getA())
                    res_a = _rhs.getA();
                else
                    res_a = _lhs.getA();

                if (_lhs.getB() > _rhs.getB())
                    res_b = _rhs.getB();
                else
                    res_b = _lhs.getB();
                if (res_a != res_b) {
                    res.second = Intersec::quantity::Interval;
                }
                else {
                    res.second = Intersec::quantity::One;
                }
            }
            else {
                //a2*---------*b2
                //          a1*-------------*b1
                res_a = res_b = _lhs.getA();
                res.second = Intersec::quantity::One;
            }
        }
        else if (_lhs.getB() == _rhs.getA()) {
            //              a2*---------*b2
            //a1*-------------*b1
            res_a = res_b = _lhs.getB();
            res.second = Intersec::quantity::One;
        }


        res.first = LineSegment1(res_a, res_b);

        return res;
    }


}//namespace la (LineSegment & LineSegment)



///////////////////////////////////////////////////////////////
//
// Plane & Plane
//
///////////////////////////////////////////////////////////////
namespace la
{
    bool intersec(const Plane& _lhs, const Plane& _rhs)
    {
        return contein(_lhs, _rhs.getP())
            && contein(_lhs, _rhs.getP() + _rhs.getA())
            && contein(_lhs, _rhs.getP() + _rhs.getB());
    }

    std::pair<Line3, Intersec::quantity> findIntersec(const Plane& _lhs, const Plane& _rhs)
    {
        auto res = std::make_pair(Line3(), Intersec::quantity::Nop);

        if (intersec(_lhs, _rhs))
        {
            //r = E + k * V - intersection line
            //
            //V = [n1 * n2];
            //
            //dot(P2 - E, n2) = 0
            //dot(P1 - E, n1) = 0
            //<=>
            //s * dot(a1, n2) + t * dot(b1, n2) = dot(P2 - P1, n2)
            //s * dot(a1, n1) + t * dot(b1, n1) = dot(P2 - P1, n1)
            //<=>
            //s * q + t * w = l
            //s * r + t * y = u
            //
            //det = q * y - w * r
            //...

            float q = _lhs.getA() ^ _rhs.getN();
            float w = _lhs.getB() ^ _rhs.getN();
            float l = (_rhs.getP() - _lhs.getP()) ^ _rhs.getN();
            float r = _lhs.getA() ^ _lhs.getN();
            float y = _lhs.getB() ^ _lhs.getN();
            float u = (_rhs.getP() - _lhs.getP()) ^ _lhs.getN();

            float det = q * y - w * r;

            if (std::abs(det) < EPSILON)
            {
                res.second = Intersec::quantity::Same;
            }
            else
            {
                Vector3f V_ = _lhs.getN() * _rhs.getN();

                float det1 = l * y - w * u;
                float det2 = q * u - r * l;

                float s = det1 / det;
                float t = det2 / det;

                res.first.reup(_lhs.getP() + s * _lhs.getA() + t * _lhs.getB(),
                    V_, Line3::Type::PointAndVector);
                res.second = Intersec::quantity::Interval;
            }
        }

        return res;
    }

    bool equal(const Plane& _lhs, const Plane& _rhs)
    {
        bool res = true;

        res = res && _lhs.getN() * _rhs.getN() == Vector3f(0.f);
        res = res && std::abs((_lhs.getP() - _rhs.getP()) ^ _lhs.getN()) < EPSILON;

        return res;
    }

    float distance(const Plane& _lhs, const Plane& _rhs)
    {
        return std::abs(distanceWithSign(_lhs, _rhs));
    }

    float distanceWithSign(const Plane& _lhs, const Plane& _rhs)
    {
        float res = 0.f;

        if (!intersec(_lhs, _rhs))
        {
            //dot(_rhs.getP() - (_lhs.getN() * s + _lhs.getP()), _rhs.getN()) = 0
            //dot(_rhs.getP(), _rhs.getN()) - dot(_lhs.getN() * s, _rhs.getN()) 
            //    - dot(_lhs.getP(), _rhs.getN()) = 0
            //
            // s * dot(_lhs.getN(), _rhs.getN()) =  dot(_rhs.getP(), _rhs.getN()) 
            //                                      - dot(_lhs.getP(), _rhs.getN())
            float a = scalarProduct(_lhs.getN(), _rhs.getN());
            float b = scalarProduct(_rhs.getP(), _rhs.getN());
            float c = scalarProduct(_lhs.getP(), _rhs.getN());

            res = (b - c) / a;
        }

        return res;
    }

}//namespace la (Plane & Plane)


///////////////////////////////////////////////////////////////
//
// Plane & Vector
//
///////////////////////////////////////////////////////////////
namespace la
{
//3D
    bool contein(const Plane& _pl, const Vector3f& _point)
    {
        return std::abs(scalarProduct(_pl.getP() - _point, _pl.getN())) < EPSILON;
    }

    Vector3f projection(const Vector3f& _point, const Plane& _pl)
    {
        Vector3f res = _point;

        float s = distanceWithSign(_pl, _point);


        return _point - _pl.getN() * s;
    }

    float distance(const Plane& _pl, const Vector3f _point)
    {
        return distance(_pl, Plane(_point, _pl.getA(), _pl.getB()));
    }

    float distanceWithSign(const Plane& _pl, const Vector3f _point)
    {
        return distanceWithSign(_pl, Plane(_point, _pl.getA(), _pl.getB()));
    }

}//namespace la (Plane & Vector)


///////////////////////////////////////////////////////////////
//
// Plane & Line
//
///////////////////////////////////////////////////////////////
namespace la
{
    bool intersec(const Plane& _pl, const Line3& _ln)
    {
        bool res = false;

        if (contein(_pl, _ln))
        {
            res = true;
        }
        else
        {
            res = std::abs(scalarProduct(_pl.getN(), _ln.getV())) > EPSILON;
        }

        return res;
    }

    Line3 projection(const Line3& _line, const Plane& _pl)
    {
        Vector3f p1_ = projection(_line.getP(), _pl);
        Vector3f p2_ = projection(_line.getP() + _line.getV(), _pl);

        return {p1_, p2_, Line3::Type::TwoPoints};
    }

    std::pair<Vector3f, Intersec::quantity>
        findIntersec(const Plane& _pl, const Line3& _ln)
    {
        auto res = std::make_pair(Vector3f(0.f), Intersec::quantity::Nop);

        if (intersec(_pl, _ln)) {
            //Q - required point
            //
            //Q = _ln.getP() + s * _ln.getV();
            //dot(_pl.getN(), Q - _pl.getP()) = 0;
            //<=>
            //dot(_pl.getN(), _ln.getP() + s * _ln.getV()) - dot(_pl.getN(), _pl.getP()) = 0;
            //<=>
            //s = [ dot(_pl.getN(), _pl.getP()) - dot(_pl.getN(), _ln.getP()) ] / dot(_pl.getN(), _ln.getV());
            //<=> 
            //s = (a - b) / c;

            float c = _pl.getN() ^ _ln.getV();

            if (std::abs(c) < EPSILON)
            {
                res.second = Intersec::quantity::Same;
            }
            else
            {
                res.second = Intersec::quantity::One;

                float a = _pl.getN() ^ _pl.getP();
                float b = _pl.getN() ^ _ln.getP();

                float s = (a - b) / c;
                res.first = _ln.getP() + s * _ln.getV();
            }
        }
        
        return res;
    }

    float distanceWithSign(const Plane& _pl, const Line3 _ln)
    {
        float res = 0.f;

        if (!_ln.getV().collinear(_pl.getA()))
        {
            res = distanceWithSign(_pl, Plane(_ln.getP(), _ln.getV(), _pl.getA()));
        }
        else
        {
            res = distanceWithSign(_pl, Plane(_ln.getP(), _ln.getV(), _pl.getB()));
        }

        return res;
    }

    float distance(const Plane& _pl, const Line3 _ln)
    {
        return std::abs(distanceWithSign(_pl, _ln));
    }

    bool contein(const Plane& _pl, const Line3& _ln)
    {
        return contein(_pl, _ln.getP()) && contein(_pl, _ln.getP() + _ln.getV());
    }

}//namespace la (Plane & Line)