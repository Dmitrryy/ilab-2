#pragma once 

#include "gtest/gtest.h"

#include "../LAL/include/LAL.h"
#include "../LAL/include/LALmath.h"

#define test_class(name)                    \
class name##Test : public ::testing::Test { \
protected:                                  \
    name##Test() = default;                 \
                                            \
    name q0_;                               \
    name q1_;                               \
    name q2_;                               \
    name q3_;                               \
};

using namespace la;

test_class(Vector2f);
test_class(Vector3f);
test_class(LineSegment1);
test_class(LineSegment2);
test_class(LineSegment3);
test_class(Line2);
test_class(Line3);
test_class(Plane);
test_class(Triangle);


TEST_F(Vector3fTest, Construct)
{
    EXPECT_EQ(q0_.x, 0.f);
    EXPECT_EQ(q0_.y, 0.f);
    EXPECT_EQ(q0_.z, 0.f);
}

TEST_F(Vector3fTest, Modul)
{
    EXPECT_EQ(q0_.modul(), 0.f);

    q0_ = { 1.f, 0.f, 0.f };
    EXPECT_EQ(q0_.modul(), 1.f);

    q0_ = { 1.f, 3.f, 0.f };
    EXPECT_EQ(q0_.modul(), std::sqrt(10.f));

    q0_ = { 1.f, 3.f, 10.f };
    EXPECT_EQ(q0_.modul(), std::sqrt(110.f));
}

TEST_F(Vector3fTest, scalarProduct)
{
    EXPECT_EQ(scalarProduct(q0_, q1_), 0.f);

    q0_ = { 11.f, 0.f, 123.f };
    q1_ = { 0.f, 1234.f, 0.f };
    EXPECT_EQ((q0_ ^ q1_), 0.f);

    q0_ = { 11.f, 0.f, 123.f };
    EXPECT_EQ(scalarProduct(q0_, q2_), 0.f);

    q0_ = { 10.f, 0.f, 0.f };
    q1_ = { 100.f, 0.f, 0.f };
    EXPECT_EQ(scalarProduct(q0_, q1_), 1000.f);

    q0_ = { 12.f, 34.f, 32.f };
    q1_ = { 12.f, 453.f, 3.f };
    EXPECT_EQ(scalarProduct(q0_, q1_), 15642.f);
}

TEST_F(Vector3fTest, crossProduct)
{
    EXPECT_EQ(crossProduct(q0_, q1_), Vector3f(0.f));

    q0_ = { 11.f, 0.f, 0.f };
    q1_ = { 1243.f, 0.f, 0.f };
    EXPECT_EQ(crossProduct(q0_, q1_), Vector3f(0.f));

    q0_ = { 213.f, 13.f, 56.f };
    q1_ = { 90.f, 346.f, 12436.f };
    EXPECT_EQ(crossProduct(q0_, q1_),
        Vector3f(142292.f, -2643828.f, 72528));
}

TEST_F(Vector3fTest, normalization)
{
    EXPECT_EQ(normalization(q0_), Vector3f(0.f));

    q0_ = { 1021.f, 0.f, 0.f };
    EXPECT_EQ(normalization(q0_), Vector3f(1.f, 0.f, 0.f));

    q0_ = { 0.01f, 0.f, 0.f };
    EXPECT_EQ(normalization(q0_), Vector3f(1.f, 0.f, 0.f));

    q0_ = { -1021.f, 0.f, 0.f };
    EXPECT_EQ(normalization(q0_), Vector3f(-1.f, 0.f, 0.f));

    q0_ = { 14.f, -17.f, 109.f };
    EXPECT_EQ(normalization(q0_),
        Vector3f(14.f / q0_.modul(), -17.f / q0_.modul(), 109.f / q0_.modul()));
}

TEST_F(Vector3fTest, collinear)
{
    EXPECT_TRUE(collinear(q0_, q1_));

    q0_ = { 1.f, 1.f, 1.f };
    q1_ = { 1.f, 1.f, 1.f };
    EXPECT_TRUE(collinear(q0_, q1_));
    EXPECT_TRUE(collinear(q0_, Vector3f(0.f)));

    q0_ = { 1.f, 1.f, 1.f };
    q1_ = { 10.f, 10.f, 10.f };
    q2_ = { 121.f, 21.f, 123.f };
    EXPECT_TRUE(collinear(q0_, q1_));
    EXPECT_FALSE(collinear(q0_, q2_));
}

TEST_F(Vector3fTest, projection)
{
    EXPECT_EQ(projection(q0_, Line3({ -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f })),
        Vector3f(0, 1, 1));

    q0_ = { 23.f, 5.f, 786.f };
    EXPECT_EQ(projection(q0_, Line3({ 34.f, 6.f, 8.f }, { 1.f, 8.f, 2.f }, Line3::Type::PointAndVector)),
        Vector3f(56.2753623f, 184.2028985f, 52.5507246f));

    q0_ = { -23.f, 90.f, -6732.f };
    EXPECT_EQ(projection(q0_, Line3({ 34.f, 6.f, 8.f }, { 1.f, 8.f, 2.f }, Line3::Type::PointAndVector)),
        Vector3f(-152.4492753f, -1485.5942028f, -364.8985507f));

    q0_ = { -23.f, 90.f, -6732.f };
    EXPECT_EQ(projection(q0_, Line3({ 34.f, 6.f, 8.f }, { -23.f, 90.f, -6732.f })),
        q0_);

    EXPECT_EQ(projection({ -6.f, 6.f }, Line2({ -2.f, 0.f }, { 0.f, 2.f })),
        Vector2f(-1, 1));
}

TEST_F(Vector3fTest, distanceToPlane)
{
    //EXPECT_EQ(distance(Plane(0, 0, 1, 4), Plane(0, 0, 1, 5)), 1);
    //EXPECT_EQ(distance(Plane(12, 3, -1, 4), Plane(12, 3, -1, 4)), 0);

    //EXPECT_TRUE(std::abs(distance(Plane(3, -5, 14, 234),
    //    Plane(-6, 10, -28, 12))) - 15.8251313614 < EPSILON);
}

TEST_F(Vector3fTest, distance)
{
    EXPECT_TRUE(distance({ 0, 0 }, Line2({ -1, 1 }, { 0, 2 })) - std::sqrt(2) < EPSILON);
}


////////////////////////////////////////////////////////
//
// Line test
//
////////////////////////////////////////////////////////


TEST_F(LineSegment1Test, intersection)
{
    q0_.reup(-90, 2);
    EXPECT_TRUE(q0_.contein(2));
    EXPECT_TRUE(q0_.contein(-90));
    EXPECT_TRUE(q0_.contein(-0));

    EXPECT_FALSE(q0_.contein(-91));
    EXPECT_FALSE(q0_.contein(3));

    q1_.reup(-10, 5);
    EXPECT_TRUE(q1_.intersection(q0_));
    EXPECT_TRUE(q0_.intersection(q1_));
    EXPECT_TRUE(q0_.intersection(q2_));

    q1_.reup(2, 5);
    EXPECT_TRUE(q1_.intersection(q0_));

    q1_.reup(3, 5);
    EXPECT_FALSE(q1_.intersection(q0_));

    q1_.reup(-100, 5);
    EXPECT_TRUE(q1_.intersection(q0_));
}

TEST_F(LineSegment1Test, findIntersection)
{
    q0_.reup(-10, 5);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment1(0.f, 0.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);

    q0_.reup(-10, 5);
    q1_.reup(-10, 5);
    EXPECT_EQ(findIntersection(q0_, q1_).first, q0_);
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);

    q0_.reup(-10, 5);
    q1_.reup(-10, -11);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment1(-10.f, -10.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);

    q0_.reup(-10, 5);
    q1_.reup(5, 10);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment1(5.f, 5.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);

    q0_.reup(-10, 5);
    q1_.reup(-100, 213);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment1(-10.f, 5.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);


    q0_.reup(-10, 5);
    q1_.reup(-100, -213);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment1(0.f, 0.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Nop);
}

TEST_F(LineSegment2Test, contein)
{
    //EXPECT_TRUE(q0_.contein(Vector2f( 0.f )));

    q0_.reup({ 1.f, 1.f }, { -1.f, -1.f });
    EXPECT_TRUE(q0_.contein(Vector2f(0.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(1.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(0.5f, 0.5f)));
    EXPECT_TRUE(q0_.contein(Vector2f(-1.f)));
    EXPECT_FALSE(q0_.contein(Vector2f(0.f, 0.001f)));
    EXPECT_FALSE(q0_.contein(Vector2f(2.f, 2.f)));

    q0_.reup({ 0.f, 2.f }, { -3.f, 0.f });
    EXPECT_FALSE(q0_.contein(Vector2f(0.f)));
    EXPECT_FALSE(q0_.contein(Vector2f(0.f, 1.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(0.f, 2.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(-3.f, 0.f)));

}

TEST_F(LineSegment2Test, findIntersection)
{
    q0_.reup({ 1.f, 1.f }, { -1.f, -1.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, q1_);


    q0_.reup({ 0.f, 1.f }, { 0.f, -10.f });
    q1_.reup({ 0.f, 1.f }, { 0.f, -10.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);
    EXPECT_EQ(findIntersection(q0_, q1_).first, q0_);

    q0_.reup({ 0.f, 1.f }, { 0.f, -10.f });
    q1_.reup({ 0.f, -100.f }, { 0.f, 10.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);
    EXPECT_EQ(findIntersection(q0_, q1_).first, q0_);

    q0_.reup({ 10.f, 0.f }, { -10.f, 0.f });
    q1_.reup({ 5.f, 0.f }, { 5.f, 10.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment2({ 5.f, 0.f }, { 5.f, 0.f }));

    q0_.reup({ -2.f, 0.f }, { 0.f, 3.f });
    q1_.reup({ 0.f, 3.f }, { 3.f, 10.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment2({ 0.f, 3.f }, { 0.f, 3.f }));

    q0_.reup({ -2.f, 0.f }, { 0.f, 3.f });
    q1_.reup({ -2.f, 0.f }, { 2.f, 6.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);
    EXPECT_EQ(findIntersection(q0_, q1_).first, q0_);

    q0_.reup({ -2.f, 0.f }, { 0.f, 3.f });
    q1_.reup({ 0.f, 0.f }, { 2.f, 6.f });
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Nop);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment2());
}

TEST_F(LineSegment3Test, contein)
{
    q0_.reup({ 1, 1, 1 }, { -5, -5, -5 });
    EXPECT_TRUE(q0_.contein({ 0, 0, 0 }));
    EXPECT_TRUE(q0_.contein({ 1, 1, 1 }));
    EXPECT_TRUE(q0_.contein({ -5, -5, -5 }));

    EXPECT_FALSE(q0_.contein({ 5, -5, -5 }));
    EXPECT_FALSE(q0_.contein({ 5, 5, 5 }));
    EXPECT_FALSE(q0_.contein({ -6, -6, -6 }));
}

TEST_F(LineSegment3Test, findintersection)
{
    //q0_.reup({ 1, 3, 1 }, { 1, 1, -2 });
    //q1_.reup({ -1, 0, -4 }, { 0, -1.5, -2 });
    //EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment3({ 1, -3, 0 }, {1, -3, 0}, LineSegment3::Type::TwoPoints));
    //EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);

    //q0_.reup({ -4, -5, 6 }, { -2, 4, 6 }, Line3::Type::PointAndVector);
    //q1_.reup({ 0, 1, -3 }, { 1, -2, -3 }, Line3::Type::PointAndVector);
    //EXPECT_EQ(findIntersection(q0_, q1_).first, Vector3f(0.f));
    //EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Nop);

    q0_.reup({ 0, 1, -3 }, { -2, 4, 6 }, LineSegment3::Type::PointAndVector);
    q1_.reup({ 0, 1, -3 }, { -1, 2, 3 }, LineSegment3::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).first, LineSegment3({ 0, 1, -3 }, { -1, 3, 0 }, LineSegment3::Type::TwoPoints));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Interval);
}

///////////////////////////////////////////////////////////
//
// Line2 test
//
///////////////////////////////////////////////////////////

TEST_F(Line2Test, contein)
{
    q0_.reup({ -1.f, 0.f }, { 0.f, 2.f });
    EXPECT_FALSE(q0_.contein(Vector2f(0.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(-2.f, -2.f)));

    q0_.reup({ -1.f, -1.f }, { 2.f, 2.f });
    EXPECT_FALSE(q0_.contein(Vector2f(-1.f, 1.f)));
    EXPECT_TRUE(q0_.contein(Vector2f(-2.f, -2.f)));
}

TEST_F(Line2Test, intersection)
{
    EXPECT_TRUE(q0_.intersection(q1_));

    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Same);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector2f(0.f));

    q0_.reup({ 0.f,0.f }, { 0.f, 10.f });
    q1_.reup({ 1.f,0.f }, { 1.f, 10.f });
    EXPECT_FALSE(q0_.intersection(q1_));

    q0_.reup({ 0.f, 0.f }, { 10.f, 10.f }, Line2::Type::PointAndVector);
    q1_.reup({ 0.f, -1.f }, { -10.f, -10.f }, Line2::Type::PointAndVector);
    EXPECT_FALSE(q0_.intersection(q1_));

    q0_.reup({ 0.f,0.f }, { 0.f, 10.f });
    q1_.reup({ 1.f,0.f }, { 1.1f, 10.f });
    EXPECT_TRUE(q0_.intersection(q1_));
}

TEST_F(Line2Test, findIntersection)
{
    q0_.reup({ 0.f,0.f }, { 1.f, 1.f }, Line2::Type::PointAndVector);
    q1_.reup({ 0.f,0.f }, { -1.f, 1.f }, Line2::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector2f(0.f));

    q0_.reup({ 0.f,0.f }, { 1.f, 1.f }, Line2::Type::PointAndVector);
    q1_.reup({ -2.f,-2.f }, { -1.f, -1.f }, Line2::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Same);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector2f(0.f));

    q0_.reup({ 12.f,34.f }, { 213.f, 7.f }, Line2::Type::PointAndVector);
    q1_.reup({ 34.f,83.f }, { 3.f, 67.f }, Line2::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector2f(31.83515f, 34.65185f));

    q0_.reup({ 8.f, 5.f }, { 4.f, 7.f }, Line2::Type::PointAndVector);
    q1_.reup({ 1.f, 4.f }, { 1.f, 10.f }, Line2::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector2f(-0.363636f, -9.636363f));
}

///////////////////////////////////////////////////////////
//
// Line2 test
//
///////////////////////////////////////////////////////////

TEST_F(Line3Test, distance)
{
    q0_.reup({ -1, 1, 0 }, { 1, 1, 0 });
    q1_.reup({ -1, -1, 0 }, { 1, -1, 0 });
    EXPECT_EQ(distance(q0_, q1_), 2);
    EXPECT_EQ(distance(q0_, q0_), 0);

    q0_.reup({ 3, 2, 4 }, { 5, 89, 3 }, Line3::Type::PointAndVector);
    q1_.reup({ 2, 4, 8 }, { 2, 3, 4 }, Line3::Type::PointAndVector);
    EXPECT_TRUE(std::abs(distance(q0_, q1_) - 2.6770402) < EPSILON);
    EXPECT_EQ(distance(q0_, q0_), 0);

    q0_.reup({ 70, 2, 4 }, { 5, 89, 3 }, Line3::Type::PointAndVector);
    q1_.reup({ 12, 14, 8 }, { 2, 3, 4 }, Line3::Type::PointAndVector);
    EXPECT_TRUE(std::abs(distance(q0_, q1_) - 54.5991086) < EPSILON);
    EXPECT_EQ(distance(q0_, q0_), 0);
}

TEST_F(Line3Test, findIntersection)
{
    q0_.reup({ 1, -1, -1 }, { 1, 1, -2 });
    q1_.reup({ -1, 0, -4 }, { 0, -1.5, -2 });
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector3f(1, -3, 0));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::One);

    q0_.reup({ -4, -5, 6 }, { -2, 4, 6 }, Line3::Type::PointAndVector);
    q1_.reup({ 0, 1, -3 }, { 1, -2, -3 }, Line3::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector3f(0.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Nop);

    q0_.reup({ 0 + 1, 1 - 2, -3 - 3 }, { -2, 4, 6 }, Line3::Type::PointAndVector);
    q1_.reup({ 0, 1, -3 }, { 1, -2, -3 }, Line3::Type::PointAndVector);
    EXPECT_EQ(findIntersection(q0_, q1_).first, Vector3f(0.f));
    EXPECT_EQ(findIntersection(q0_, q1_).second, Intersec::quantity::Same);
}

///////////////////////////////////////////////////////////
//
// Plane test
//
///////////////////////////////////////////////////////////

TEST_F(PlaneTest, constructor)
{
    q0_.reup(Vector3f(1, 2, 56), Vector3f(-9, 6, 23), Vector3f(4, -3, 3), Plane::Type::ThreePoints);
    EXPECT_EQ(Vector3f(scalarProduct(q0_.getA(), q0_.getB())), Vector3f(0.f));
    //q1_.reup(-381, -639, 38, -469);
    //EXPECT_EQ(q0_, q1_);
}

TEST_F(PlaneTest, intersec)
{
    //q0_.reup(1, 2, 3, -203);
    //q1_.reup(-1, -2, -3, 123);
    //q2_.reup(2, 4, 5, 1);
    //EXPECT_FALSE(intersec(q0_, q1_));
    //EXPECT_TRUE(intersec(q0_, q2_));
}

TEST_F(PlaneTest, findIntersec)
{
    //q0_.reup(9, -3, 8, 2);
    //q1_.reup(-12, 2, -45, 1);

    //EXPECT_EQ(findIntersec(q0_, q0_).second, Intersec::quantity::Same);
    //EXPECT_EQ(findIntersec(q0_, q0_).first, Line3());

    //EXPECT_FALSE(Line3({ 1, 1, 1 }, { 2, 2, 2 }) == Line3({ -1, 1, 1 }, { 0 , 0, 0 }));

    //EXPECT_EQ(findIntersec(q1_, q0_).second, Intersec::quantity::Interval);
    //EXPECT_EQ(findIntersec(q1_, q0_).first, Line3(Vector3f(0.3888888, 1.833333, 0),
    //    Vector3f(-6.111111, -17.16666, 1), Line3::Type::PointAndVector));
}

TEST_F(PlaneTest, projection)
{
    //q0_.reup(9, 2, 5, 10);
    //EXPECT_EQ(projection(Vector3f(213, -21, 9), q0_), Vector3f(55.090909, -56.090909, -78.727272));

    //q1_.reup(3, 5, 6, 0);
    //EXPECT_EQ(projection(Vector3f(2, 0, -1), q1_), Vector3f(2, 0, -1));

}


///////////////////////////////////////////////////////////
//
// Triangle test
//
///////////////////////////////////////////////////////////

TEST_F(TriangleTest, construct)
{
    q0_.reup({ 1, 1, 1 }, { 2, 2, 2 }, { 1, 0, 0 });
    q1_.reup({ 1, 0, 0 }, { 2, 2, 2 }, { 1, 1, 1 });
    EXPECT_EQ(q0_, q1_);
}

TEST_F(TriangleTest, intersec)
{
    q0_.reup({ 1, 1, 0 }, { 2, 2, 0 }, { 6, 0, 0 });
    q0_.reup({ 1, 1, 1 }, { 2, 2, 1 }, { 6, 0, 1 });
    EXPECT_FALSE(intersec(q0_, q1_));
}