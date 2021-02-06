#include <gtest/gtest.h>

#include "graph/graph.h"

using namespace ezg;

class GraphTest : public ::testing::Test
{
protected:

    Graph_t< size_t > q0_;
    Graph_t< char > q1_;
};


TEST_F(GraphTest, construct)
{
    EXPECT_EQ(q0_.vertCapacity(), 4);
    EXPECT_EQ(q0_.vertSize(), 0);
    EXPECT_EQ(q0_.edgeSize(), 0);
}


//todo