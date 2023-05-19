#include "gtest/gtest.h"
#include "6_lambda_expressions.h"

#include <vector>
#include <algorithm>
#include <functional>

TEST(C6_lambda_expressions, CaptureByValue)
{
    int x = 5;
    auto addValue = [x] (int some) {
        return x + some;
    };
    x = 10;
    auto res = caller(addValue, 10);
    EXPECT_EQ(res, 15);
}

TEST(C6_lambda_expressions, CaptureByRef)
{
    int x = 5;
    auto addValue = [&x] (int some) {
        return x + some;
    };
    x = 10;
    auto res = caller(addValue, 10);
    EXPECT_EQ(res, 20);
}

TEST(C6_lambda_expressions, CaptureByMoveWithLambda)
{
    std::vector<int> vec = {1,2,3,-1,0};
    auto contains = [myvec = std::move(vec)] (int some) {
        return std::find(myvec.cbegin(), myvec.cend(), some) != myvec.cend();
    };
    EXPECT_TRUE(vec.empty());
    EXPECT_TRUE(caller(contains, 3));
    EXPECT_FALSE(caller(contains, 10));
}

template<typename T>
inline bool containsIn(const std::vector<T>& vec, const T& elem) noexcept
{
    return std::find(vec.cbegin(), vec.cend(), elem) != vec.cend();
};

TEST(C6_lambda_expressions, CaptureByMoveWithBind)
{
    using namespace std::placeholders;
    std::vector<int> vec = {1,2,3,-1,0};
    auto containsB = std::bind(containsIn<int>,
        std::move(vec),
        _1
    );

    EXPECT_TRUE(vec.empty());
    EXPECT_TRUE(containsB(3));
    EXPECT_FALSE(containsB(10));
}

TEST(C6_lambda_expressions, CorrectCaptureFromContext)
{
    Context c;
    auto lambda1 = c.getLambdaWithCapturedContext();
    c.changeContext(35);
    auto lambda2 = c.getLambdaWithCapturedContext();
    EXPECT_EQ(caller(lambda1), 42);
    EXPECT_EQ(caller(lambda2), 35);
}

TEST(C6_lambda_expressions, TemplatedLambdaExpression)
{
    EXPECT_EQ(avgL(5,10), 7);
    EXPECT_NE(avgL(2147483647, 1), 1073741824);
    EXPECT_EQ(avgL(2147483647LL, 1LL), 1073741824LL);
}