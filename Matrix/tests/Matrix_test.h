#pragma once 

#include "gtest/gtest.h"

#include "../matrix/Matrix.h"

using namespace matrix;

class MatrixTest : public ::testing::Test
{
protected:
	MatrixTest()
		: q0_()
		, q1_(0, 0, Order::Row)
		, q2_(10, 10, Order::Column)
		, q3_(10, 10, Order::Row)
		, q4_(3, 6, Order::Column)
	{}


	Matrix<double> q0_;
	Matrix<double> q1_;
	Matrix<double> q2_;
	Matrix<double> q3_;
	Matrix<double> q4_;
};


TEST_F(MatrixTest, construct)
{
	EXPECT_EQ(q0_, q1_);
	EXPECT_EQ(q2_, q3_);

	EXPECT_TRUE(q2_ == q2_);
	EXPECT_TRUE(q2_ == q3_);

	q4_ = q3_;
	EXPECT_TRUE(q4_ == q3_);

	q3_.at(9, 9) = 10;
	EXPECT_FALSE(q4_ == q3_);

	q4_.at(9, 9) = 10;
	EXPECT_TRUE(q3_ == q4_);
}

TEST_F(MatrixTest, setOrder)
{
	q4_ = q3_;
	EXPECT_TRUE(q4_ == q3_);

	q3_.at(9, 9) = 10;
	EXPECT_FALSE(q4_ == q3_);

	q4_.at(9, 9) = 10;
	EXPECT_TRUE(q3_ == q4_);

	q3_.setOrder(Order::Column);
	EXPECT_TRUE(q3_ == q4_);

}

TEST_F(MatrixTest, submatrix)
{

}