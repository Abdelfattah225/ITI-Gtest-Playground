#include <gtest/gtest.h>

// Interface for a payment processor
class IPaymentProcessor
{
public:
    virtual ~IPaymentProcessor() = default;
    virtual bool processPayment(double amount) = 0;
};

// TASK: Create FakePaymentProcessor
// - Has a boolean member 'shouldSucceed'
// - Has a method 'setShouldSucceed(bool value)'
// - processPayment returns the value of 'shouldSucceed'

class FakePaymentProcessor : public IPaymentProcessor
{
    // YOUR CODE HERE
private:
    bool shouldSucceed;

public:
    void setShouldSucceed(bool value)
    {
        shouldSucceed = value;
    }
    bool processPayment(double amount) override
    {
        return shouldSucceed;
    }
};

// Class under test
class ShoppingCart
{
private:
    IPaymentProcessor *processor;
    double total;

public:
    ShoppingCart(IPaymentProcessor *p) : processor(p), total(0) {}

    void addItem(double price) { total += price; }

    bool checkout()
    {
        if (total <= 0)
            return false;
        return processor->processPayment(total);
    }

    double getTotal() { return total; }
};

// TASK: Create Test Fixture called ShoppingCartTest
// YOUR CODE HERE
class ShoppingCartTest : public ::testing::Test
{

protected:
    FakePaymentProcessor fakeprocessor;
    ShoppingCart *cart;
    void SetUp() override
    {
        cart = new ShoppingCart(&fakeprocessor);
    }
    void TearDown() override
    {
        delete cart;
    }
};
// TASK: Write these tests:
// 1. CheckoutSucceedsWhenPaymentSucceeds
//    - Add item for $50
//    - Set fake to succeed
//    - Check that checkout() returns true

TEST_F(ShoppingCartTest, CheckoutSucceedsWhenPaymentSucceeds)
{
    cart->addItem(50);
    fakeprocessor.setShouldSucceed(true);
    EXPECT_TRUE(cart->checkout());
}

// 2. CheckoutFailsWhenPaymentFails
//    - Add item for $50
//    - Set fake to fail
//    - Check that checkout() returns false

TEST_F(ShoppingCartTest, CheckoutFailsWhenPaymentFails)
{
    cart->addItem(50);
    fakeprocessor.setShouldSucceed(false);
    EXPECT_FALSE(cart->checkout());
}

// 3. CheckoutFailsWhenCartEmpty
//    - Don't add any items
//    - Check that checkout() returns false

TEST_F(ShoppingCartTest, CheckoutFailsWhenCartEmpty)
{   cart->addItem(50);
    fakeprocessor.setShouldSucceed(false);
    EXPECT_FALSE(cart->checkout());
}