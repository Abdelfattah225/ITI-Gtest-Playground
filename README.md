# Google Test (gtest) Complete Guide 📚

Here's your personal reference guide!

---

```markdown
# Google Test (gtest) Complete Reference Guide

A comprehensive guide to C++ unit testing with Google Test framework.

## Table of Contents

1. [Setup & Compilation](#setup--compilation)
2. [Level 1: Why Unit Testing?](#level-1-why-unit-testing)
3. [Level 2: Anatomy of TEST()](#level-2-anatomy-of-test)
4. [Level 3: Assertions (EXPECT vs ASSERT)](#level-3-assertions-expect-vs-assert)
5. [Level 4: Test Fixtures](#level-4-test-fixtures)
6. [Level 5: Test Doubles (Fakes, Stubs, Mocks)](#level-5-test-doubles)
7. [Level 6: Real Project Example](#level-6-real-project-example)
8. [Quick Reference Cheat Sheet](#quick-reference-cheat-sheet)
9. [Test Case Generation Strategies](#test-case-generation-strategies)

---

## Setup & Compilation

### Basic Compilation Command

```bash
g++ main.cpp -L/usr/local/lib -lgtest -lgtest_main -pthread -o test_app
./test_app
```

### Minimal Test File Structure

```cpp
#include <gtest/gtest.h>

// Your code or #include your header files here

TEST(TestSuiteName, TestName) {
    // Test body
}

// No main() needed - gtest_main provides it
```

---

## Level 1: Why Unit Testing?

### What is a Unit Test?

A small piece of code that verifies a small piece of your code works correctly.

```
Your Code          Your Test
┌──────────┐       ┌─────────────────────┐
│ add(a,b) │ ◄──── │ "Give 2,3 expect 5" │
└──────────┘       └─────────────────────┘
```

### 5 Key Benefits

| Benefit | Description |
|---------|-------------|
| **Confidence** | Safely modify code knowing tests catch breaks |
| **Documentation** | Tests show how code should be used |
| **Early Bug Detection** | Find bugs in minutes, not days |
| **Better Design** | Testable code = well-designed code |
| **Fearless Refactoring** | Improve code structure safely |

### The Testing Mindset

```
Unit Test = Contract/Promise

"This function PROMISES that when given X, it will return Y"
```

---

## Level 2: Anatomy of TEST()

### Basic Structure

```cpp
TEST(TestSuiteName, TestName) {
    // Arrange - Setup (optional)
    // Act     - Call the code
    // Assert  - Verify the result
}
```

### Breaking Down Each Part

```cpp
#include <gtest/gtest.h>

// Function to test
int add(int a, int b) {
    return a + b;
}

// The test
TEST(AdditionTest, TwoPositiveNumbers) {
    int result = add(2, 3);    // ACT
    EXPECT_EQ(result, 5);      // ASSERT
}
```

### Naming Convention

```
TEST(TestSuiteName, TestName)
      │              │
      │              └── Specific scenario being tested
      └── Category/group (usually function or class name)

Full name: TestSuiteName.TestName
Example:   AdditionTest.TwoPositiveNumbers
```

### Good vs Bad Names

```cpp
// ❌ Bad
TEST(Test1, Test2) { ... }
TEST(Math, Works) { ... }

// ✅ Good
TEST(AdditionTest, TwoPositiveNumbers) { ... }
TEST(AdditionTest, NegativeNumbers) { ... }
TEST(AdditionTest, BothZero) { ... }
```

### AAA Pattern (Arrange, Act, Assert)

```cpp
TEST(CalculatorTest, DivisionWorks) {
    // ARRANGE - Setup
    Calculator calc;
    int a = 10;
    int b = 2;
    
    // ACT - Execute
    int result = calc.divide(a, b);
    
    // ASSERT - Verify
    EXPECT_EQ(result, 5);
}
```

---

## Level 3: Assertions (EXPECT vs ASSERT)

### The Two Families

| EXPECT_* | ASSERT_* |
|----------|----------|
| Non-fatal failure | Fatal failure |
| Test continues running | Test stops immediately |
| See all failures | Stop at first failure |
| Use most of the time | Use when continuing is dangerous |

### Visual Difference

```cpp
// EXPECT: continues after failure
TEST(Example, ExpectContinues) {
    EXPECT_EQ(1, 2);    // Fails, but continues
    EXPECT_EQ(3, 4);    // Also runs (and fails)
    EXPECT_EQ(5, 5);    // Also runs (passes)
}
// Result: 2 failures reported

// ASSERT: stops at failure
TEST(Example, AssertStops) {
    ASSERT_EQ(1, 2);    // Fails and STOPS
    ASSERT_EQ(3, 4);    // Never runs!
    ASSERT_EQ(5, 5);    // Never runs!
}
// Result: 1 failure reported
```

### When to Use ASSERT

```cpp
TEST(PointerTest, SafeDereference) {
    int* ptr = getPointer();
    
    ASSERT_NE(ptr, nullptr);  // MUST stop if null
    
    EXPECT_EQ(*ptr, 42);      // Would crash if ptr is null!
}
```

### Common Assertions Reference

#### Boolean

```cpp
EXPECT_TRUE(condition);      // condition == true
EXPECT_FALSE(condition);     // condition == false
```

#### Equality

```cpp
EXPECT_EQ(val1, val2);       // val1 == val2
EXPECT_NE(val1, val2);       // val1 != val2 (Not Equal)
```

#### Comparison

```cpp
EXPECT_LT(val1, val2);       // val1 < val2  (Less Than)
EXPECT_LE(val1, val2);       // val1 <= val2 (Less or Equal)
EXPECT_GT(val1, val2);       // val1 > val2  (Greater Than)
EXPECT_GE(val1, val2);       // val1 >= val2 (Greater or Equal)
```

#### Strings

```cpp
EXPECT_STREQ(str1, str2);    // C-strings equal
EXPECT_STRNE(str1, str2);    // C-strings not equal
```

### Memory Trick

```
EXPECT = "I expect this, but show me everything else too"
ASSERT = "This MUST be true, or nothing else matters"
```

---

## Level 4: Test Fixtures

### The Problem: Repeated Setup

```cpp
// ❌ Without fixture - repeated code
TEST(BankTest, Deposit) {
    BankAccount acc;
    acc.setOwner("Alice");
    acc.setBalance(100);
    // test...
}

TEST(BankTest, Withdraw) {
    BankAccount acc;           // Same setup
    acc.setOwner("Alice");     // repeated
    acc.setBalance(100);       // every time!
    // test...
}
```

### The Solution: Test Fixture

```cpp
// ✅ With fixture - clean and DRY
class BankAccountTest : public ::testing::Test {
protected:
    BankAccount account;
    
    void SetUp() override {
        account.setOwner("Alice");
        account.setBalance(100);
    }
    
    void TearDown() override {
        // Cleanup if needed
    }
};

TEST_F(BankAccountTest, Deposit) {
    account.deposit(50);
    EXPECT_EQ(account.getBalance(), 150);
}

TEST_F(BankAccountTest, Withdraw) {
    account.withdraw(30);
    EXPECT_EQ(account.getBalance(), 70);
}
```

### Fixture Anatomy

```cpp
class MyTestFixture : public ::testing::Test {
protected:
    // Shared variables
    MyClass obj;
    int testData;
    
    // Runs BEFORE each test
    void SetUp() override {
        obj = MyClass();
        testData = 42;
    }
    
    // Runs AFTER each test
    void TearDown() override {
        // Cleanup: close files, free memory, etc.
    }
    
    // Helper methods (optional)
    void helperFunction() { ... }
};
```

### TEST vs TEST_F

```cpp
TEST(SuiteName, TestName)     // No fixture
TEST_F(FixtureName, TestName) // Uses fixture (F = Fixture)
      │
      └── Must match fixture class name exactly!
```

### Key Concept: Fresh Fixture Per Test

```
Test 1 runs:
  1. Create new fixture object
  2. Call SetUp()
  3. Run test body
  4. Call TearDown()
  5. Destroy fixture object

Test 2 runs:
  1. Create NEW fixture object  ← Fresh start!
  2. Call SetUp()
  3. Run test body
  4. Call TearDown()
  5. Destroy fixture object
```

**Tests are isolated - they don't affect each other!**

### Common Mistake

```cpp
class MyFixture : public ::testing::Test { ... };

TEST(MyFixture, TestName) { ... }   // ❌ WRONG! Uses TEST
TEST_F(MyFixture, TestName) { ... } // ✅ CORRECT! Uses TEST_F
```

---

## Level 5: Test Doubles

### The Problem: External Dependencies

```cpp
class WeatherApp {
    WeatherService* service;  // Real API - slow, unpredictable!
    
    std::string getAdvice() {
        int temp = service->getCurrentTemperature();  // Network call!
        if (temp > 30) return "Stay cool!";
        // ...
    }
};
```

**Problems with real dependencies:**
- ❌ Slow (network calls)
- ❌ Unpredictable results
- ❌ Needs internet
- ❌ Can't test all scenarios

### The Solution: Test Doubles

```
Real Service  →  Test Double (Fake)
     │                  │
     │                  └── You control it!
     └── Unpredictable
```

### Types of Test Doubles

```
┌─────────┬─────────────────────────────────────────────┐
│  STUB   │ Returns pre-programmed answers              │
│         │ Does nothing - just satisfies interface     │
├─────────┼─────────────────────────────────────────────┤
│  FAKE   │ Working but simplified implementation       │
│         │ Has real logic, stores real data            │
│         │ Example: In-memory database                 │
├─────────┼─────────────────────────────────────────────┤
│  MOCK   │ Records calls and verifies interactions     │
│         │ Uses framework (Google Mock)                │
└─────────┴─────────────────────────────────────────────┘
```

### Creating a Fake (Step by Step)

#### Step 1: Define Interface

```cpp
class IWeatherService {
public:
    virtual ~IWeatherService() = default;
    virtual int getCurrentTemperature() = 0;
};
```

#### Step 2: Create Fake Implementation

```cpp
class FakeWeatherService : public IWeatherService {
private:
    int fakeTemperature;
    
public:
    void setTemperature(int temp) {
        fakeTemperature = temp;
    }
    
    int getCurrentTemperature() override {
        return fakeTemperature;  // Returns what WE set!
    }
};
```

#### Step 3: Use Dependency Injection

```cpp
class WeatherApp {
private:
    IWeatherService* service;  // Interface, not concrete class
    
public:
    WeatherApp(IWeatherService* svc) : service(svc) {}  // Inject dependency
    
    std::string getAdvice() {
        int temp = service->getCurrentTemperature();
        if (temp > 30) return "Stay cool!";
        if (temp < 10) return "Wear jacket!";
        return "Nice weather!";
    }
};
```

#### Step 4: Test with Fake

```cpp
class WeatherAppTest : public ::testing::Test {
protected:
    FakeWeatherService fakeService;
    WeatherApp* app;
    
    void SetUp() override {
        app = new WeatherApp(&fakeService);
    }
    
    void TearDown() override {
        delete app;
    }
};

TEST_F(WeatherAppTest, HotWeatherAdvice) {
    fakeService.setTemperature(35);  // Control the weather!
    EXPECT_EQ(app->getAdvice(), "Stay cool!");
}

TEST_F(WeatherAppTest, ColdWeatherAdvice) {
    fakeService.setTemperature(5);
    EXPECT_EQ(app->getAdvice(), "Wear jacket!");
}
```

### Visual: Production vs Testing

```
PRODUCTION:
┌─────────────┐         ┌──────────────────┐
│ WeatherApp  │────────►│ RealWeatherAPI   │──► Internet
└─────────────┘         └──────────────────┘

TESTING:
┌─────────────┐         ┌──────────────────┐
│ WeatherApp  │────────►│ FakeWeatherAPI   │──► In-memory (you control!)
└─────────────┘         └──────────────────┘
```

### When to Use Test Doubles

```
| Dependency | Use Double? | Reason |
|------------|-------------|--------|
| Database | ✅ Yes | Slow, needs setup |
| Network/API | ✅ Yes | Slow, unreliable |
| File system | ✅ Yes | Creates real files |
| Current time | ✅ Yes | Unpredictable |
| Random numbers | ✅ Yes | Unpredictable |
| Simple math | ❌ No | Fast, predictable |
| Pure functions | ❌ No | No side effects |
```


---

## Quick Reference Cheat Sheet

### Test Structure

```cpp
// Simple test
TEST(SuiteName, TestName) {
    EXPECT_EQ(actual, expected);
}

// Test with fixture
class MyFixture : public ::testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override { }
};

TEST_F(MyFixture, TestName) {
    EXPECT_EQ(actual, expected);
}
```

### All Assertions

```cpp
// Boolean
EXPECT_TRUE(x);
EXPECT_FALSE(x);

// Equality
EXPECT_EQ(a, b);    // a == b
EXPECT_NE(a, b);    // a != b

// Comparison
EXPECT_LT(a, b);    // a < b
EXPECT_LE(a, b);    // a <= b
EXPECT_GT(a, b);    // a > b
EXPECT_GE(a, b);    // a >= b

// Strings (C-style)
EXPECT_STREQ(s1, s2);
EXPECT_STRNE(s1, s2);

// Replace EXPECT_ with ASSERT_ for fatal failures
```

### Compilation

```bash
g++ test.cpp -L/usr/local/lib -lgtest -lgtest_main -pthread -o test
./test
```

---

## Test Case Generation Strategies

### 1. Start with Requirements

```
"What should this function DO?"

Function: divide(a, b)

Should:
├── Return correct result         → Test: NormalDivision
├── Handle division by zero       → Test: DivisionByZero
├── Work with negatives           → Test: NegativeNumbers
└── Handle zero numerator         → Test: ZeroNumerator
```

### 2. Boundary Value Analysis

```
Function: isValidAge(age)  // Valid: 0-150

Boundaries to test:
├── -1   → Below minimum (invalid)
├── 0    → At minimum (valid)
├── 1    → Above minimum (valid)
├── 75   → Middle (valid)
├── 149  → Below maximum (valid)
├── 150  → At maximum (valid)
└── 151  → Above maximum (invalid)
```

### 3. Equivalence Partitioning

```
Function: absolute(n)

Categories:
├── Positive → Test with 5 (represents all positives)
├── Negative → Test with -3 (represents all negatives)
└── Zero     → Test with 0
```

### 4. Test Case Checklist

```
For any function, consider:

✓ Happy Path      - Normal expected usage
✓ Edge Cases      - Boundaries, limits
✓ Error Cases     - Invalid input
✓ Zero/Null/Empty - 0, nullptr, ""
✓ Negative Values - If applicable
✓ Large Values    - Overflow potential
✓ Special Cases   - Domain-specific scenarios
```

---

## Summary

| Level | Concept | Key Takeaway |
|-------|---------|--------------|
| 1 | Why Test? | Catch bugs early, refactor with confidence |
| 2 | TEST() | Arrange-Act-Assert pattern |
| 3 | Assertions | EXPECT continues, ASSERT stops |
| 4 | Fixtures | Shared setup with SetUp()/TearDown() |
| 5 | Test Doubles | Fakes control unpredictable dependencies |
| 6 | Real Project | Combine all concepts together |

---

## Notes

- Created during Google Test learning journey
- Ubuntu Linux environment
- Google Test version 1.17.0

---

*Happy Testing!* 🧪✅
```

```
ITT-Gtest-playground
├── Self-Study-Gtest/
├    ├── main.cpp
├    ├── absolute_test.cpp
├    ├── test_fixture.cpp
├    ├── double_fake.cpp
├    └── project_library.cpp
└── Session_Utest   
    ├── build/
    ├── CMakeList.txt
    └── hello_test.cpp
```
