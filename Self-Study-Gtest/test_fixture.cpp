#include <gtest/gtest.h>

class Counter {
private:
    int count;
    
public:
    Counter() : count(0) {}
    
    void increment() { count++; }
    void decrement() { count--; }
    void reset() { count = 0; }
    int getCount() { return count; }
    void setCount(int value) { count = value; }
};


class CounterTest : public ::testing::Test{

    protected:
        Counter cnt;
        void SetUp() override{
            cnt.setCount(10);
        }


};

TEST_F(CounterTest,IncrementAddsOne){
    cnt.increment();
    EXPECT_EQ(cnt.getCount(),11);
}

TEST_F(CounterTest,DecrementSubtractsOne){
    cnt.decrement();
    EXPECT_EQ(cnt.getCount(),9);
}

TEST_F(CounterTest,ResetSetsToZero){
    cnt.reset();
    EXPECT_EQ(cnt.getCount(),0);
}
