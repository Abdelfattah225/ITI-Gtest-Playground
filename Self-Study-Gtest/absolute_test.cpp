#include <gtest/gtest.h>

int absolute(int n) {
    if (n < 0) return -n;
    return n;
}


TEST(AbsoluteTest,PositiveNumber){
    int result = absolute(4);
    EXPECT_EQ(result,4);
}

TEST(AbsoluteTest,NegativeNumber){
    int result = absolute(-2);
    EXPECT_EQ(result,2);
}

TEST(AbsoluteTest,NumberIsZero){
    int result = absolute(0);
    EXPECT_EQ(result,0);
}


