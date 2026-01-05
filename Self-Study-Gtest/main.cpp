#include <gtest/gtest.h>

// The function we want to test
int add(int a, int b) {
    return a * b;
}

// The test
TEST(AdditionTest, TwoPositiveNumbers) {
    int result = add(2, 3);
    EXPECT_EQ(result, 5);
}