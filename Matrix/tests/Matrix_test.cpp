#pragma once 

#include <gtest/gtest.h>
#include <gen_test.h>

#include <Matrix.h>

using namespace matrix;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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

TEST_F(MatrixTest, muvConstruct)
{
    q0_ = genMatrix(4, MType::MulUpLowTriangles);
    q1_ = q0_;
    q2_ = std::move(q0_);

    EXPECT_EQ(q1_, q2_);

    q0_ = q1_;
    q3_ = std::move(q0_) = std::move(q1_);
    EXPECT_EQ(q3_, q2_);

    q1_ = genMatrix(4, MType::MulUpLowTriangles);
    q0_ = q1_;
    Matrix tmp(std::move(q1_));
    EXPECT_EQ(q0_, tmp);
}

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


	q1_ = genMatrix(10, MType::MulUpLowTriangles);
	q2_ = q1_;

	Matrix<int> tmp(q1_);
	EXPECT_EQ(tmp, q1_);
	q1_.at(0, 0)++;
	EXPECT_TRUE(tmp != q1_);

	q0_.setOrder(Order::Row);
	q0_ = {
            {1, 2, 3},
            {5, 4, 33}
	};
	q1_.setOrder(Order::Column);
    q1_ = {
            {1, 2, 3},
            {5, 4, 33}
    };
    EXPECT_EQ(q0_, q1_);

	EXPECT_EQ(q0_.getLines(), 2);
	EXPECT_EQ(q0_.getColumns(), 3);
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
	q0_ = {
		{ 1, 2, 3},
		{ 4, 5, 6},
		{ 7, 8, 9}
	};

	q1_ = {
		{ 1, 3 },
		{ 7, 9 }
	};
	q2_ = q0_.submatrix(1, 1);
	EXPECT_EQ(q2_, q1_);

	q1_ = {
		{5, 6},
		{8, 9}
	};
	q2_ = q0_.submatrix(0, 0);
	EXPECT_EQ(q2_, q1_);

	q1_ = {
		{1, 2},
		{4, 5}
	};
	q2_ = q0_.submatrix(2, 2);
	EXPECT_EQ(q2_, q1_);

	q1_ = {
		{4, 5},
		{7, 8}
	};
	q2_ = q0_.submatrix(0, 2);
	EXPECT_EQ(q2_, q1_);
}


TEST_F(MatrixTest, determSloww)
{
	//EXPECT_EQ(q1_.determinanteSloww(), q2_.determinanteSloww());

	q0_ = {
		{1, 1},
		{1, 1}
	};
	EXPECT_EQ(q0_.determinanteSloww(), 0);

	q0_ = {
	{1, 2},
	{1, 1}
	};
	EXPECT_EQ(q0_.determinanteSloww(), -1);

	q0_ = {
	{0, 0},
	{1, 1}
	};
	EXPECT_EQ(q0_.determinanteSloww(), 0);

	q0_ = {
		{2, 3, 4},
		{8, 4, 5},
		{8, 5, 7}
	};
	EXPECT_EQ(q0_.determinanteSloww(), -10);

	q0_ = {
		{2, 3, 4},
		{0, 4, 5},
		{0, 0, 7}
	};
	EXPECT_EQ(q0_.determinanteSloww(), 56);
}

TEST_F(MatrixTest, determ)
{
	q0_ = {
		{1, 2},
		{3, 4}
	};
	EXPECT_EQ(q0_.determinante(), -2);
}

TEST_F(MatrixTest, swap)
{
	q0_ = {
	{2, 3, 4},
	{0, 4, 5},
	{0, 0, 7}
	};
	q1_ = {
	{2, 3, 4},
	{0, 0, 7},
	{0, 4, 5}
	};
	q0_.swopLines(1, 2);
	EXPECT_EQ(q0_, q1_);
}

TEST_F(MatrixTest, transpose)
{
	q0_ = {
		{0, 0},
		{1, 1}
	};
	q1_ = {
		{0, 1},
		{0, 1}
	};
	EXPECT_EQ(q0_, q1_.transpose());

	q0_ = {
		{2, 3, 4},
		{0, 4, 5},
		{0, 0, 7}
	};
	q1_ = {
		{2, 0, 0},
		{3, 4, 0},
		{4, 5, 7}
	};
    EXPECT_EQ(q0_.transpose(), q1_);

	q0_ = {
		{2, 3, 4},
		{0, 4, 5},
		{0, 0, 7}
	};
	q1_ = q0_;
	EXPECT_EQ(q0_, q1_.transpose().transpose());
}

TEST_F(MatrixTest, negate)
{
	q0_ = {
		{1, 1},
		{1, 1}
	};
	q1_ = {
		{-1, -1},
		{-1, -1}
	};
	EXPECT_EQ(q1_, q0_.negate());
}

TEST_F(MatrixTest, addSub)
{
	q0_ = {
		{0, 0},
		{1, 1}
	};
	q3_ = q0_;
	q1_ = {
		{0, 1},
		{0, 1}
	};
	q2_ = {
		{0, 1},
		{1, 2}
	};
	EXPECT_EQ(q0_.add(q1_), q2_);
	EXPECT_EQ(q0_.sub(q1_), q3_);
}

TEST_F(MatrixTest, FOREACHLOOP)
{
    q0_ = {
            {1, 2, 3},
            {4, 5, 6}
    };

    double i = 1;
    for (auto elem : q0_)
    {
        EXPECT_EQ(i, elem.val);
        if (i == 6)
        {
            EXPECT_EQ(1, elem.line);
            EXPECT_EQ(2, elem.column);
        }

        i++;
    }
}

TEST_F(MatrixTest, multipl)
{
    EXPECT_EQ(q0_.multiplication(q1_), q0_);

    q0_ = {
            {2, 0},
            {-1, 3}
    };
    q1_ = {
            {4, 0},
            {-5, 9}
    };
    EXPECT_EQ(q0_.multiplication(q0_), q1_);

    q4_ = Matrix<double>::identity(3);
    EXPECT_EQ(q4_.multiplication(q4_), q4_);

    q1_ = {
            {2, 0},
            {1, 1}
    };
    q2_ = {
            {2, 1},
            {0, 1}
    };
    q3_ = {
            {4, 2},
            {2, 2}
    };
    EXPECT_EQ(q1_.multiplication(q2_), q3_);
}