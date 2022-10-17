struct Nested
{
    struct
    {
        int x = 0;
        int y = 0;
    } point;
    int z = 0;
};

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(Tuple, NestedStruct)
{
    auto n = Nested { };
    EXPECT_EQ(n.point.x, 0);
    EXPECT_EQ(n.point.y, 0);
    EXPECT_EQ(n.z, 0);
}

TEST(Tuple, GetByPosition)
{
    // Show an example of how to get tuple elements by position.
}

TEST(Tuple, GetByType)
{
    // Show an example of how to get tuple elements by type.
}

TEST(Tuple, Assignment)
{
    // Show an example of using a tuple for assignment without temporary values
    // Fibonacci?
}