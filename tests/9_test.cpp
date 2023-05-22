#include "9_coroutines.h"
#include "gtest/gtest.h"
#include "MemoryLeakDetector.h"
#include <sstream>

TEST(Coroutines, InititalSuspendNeverStartsCoroutine)
{
    MemoryLeakDetector d;
    testing::internal::CaptureStdout();
    helloNeverSuspend();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(HELLO_FROM_CO_1, output.c_str());
}

TEST(Coroutines, InititalSuspendAlwaysDoesNotStartCoroutine)
{
    MemoryLeakDetector d;
    testing::internal::CaptureStdout();

    auto coReturnUnused = helloSuspend();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(EMPTY, output.c_str());
}

TEST(Coroutines, CoroutineStartsCorrectlyWhenResumeIsCalled)
{
    MemoryLeakDetector d;
    testing::internal::CaptureStdout();

    auto coReturn = helloSuspend();
    coReturn.resume();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(HELLO_FROM_CO_2, output.c_str());
}

TEST(Coroutines, CorrectlyPassesValueAndPrintsIt)
{
    MemoryLeakDetector d;
    testing::internal::CaptureStdout();
    const char* printedString = "some string";
    auto c1 = printMyStringOnce();
    c1.provide(printedString);
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(printedString, output.c_str());
}

TEST(Coroutines, WhenPassedSecondTimeNothingIsPrinted)
{
    MemoryLeakDetector d;
    testing::internal::CaptureStdout();
    const char* printedString = "some string";
    auto c1 = printMyStringOnce();
    c1.provide(printedString);
    c1.provide("this will not be printed because coroutine is done!");
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(printedString, output.c_str());
}

TEST(Coroutines, CorrectlyYieldsValueFromStream)
{
    MemoryLeakDetector d;
    std::istringstream stream;
    std::string first = "first";
    std::string second = "second";
    std::string third = " third ";
    std::string input { first + "\n" + second + "\n" + third };
    stream.str(input);

    auto streamGen = readStream(stream);
    streamGen.next();
    auto firstInput  = streamGen.value();
    EXPECT_STREQ(first.c_str(), firstInput.c_str());
    streamGen.next();
    auto secondInput = streamGen.value();
    EXPECT_STREQ(second.c_str(), secondInput.c_str());
    streamGen.next();
    auto thirdInput  = streamGen.value();
    EXPECT_STREQ(third.c_str(), thirdInput.c_str());
}

TEST(Coroutines, GeneratesOneZeroWithAllDefaultParams)
{
    auto gen = range<int>();
    std::vector<int> res;
    while(gen.next())
    {
        res.emplace_back(gen.value());
    }
    EXPECT_EQ(std::vector {0}, res);
}

TEST(Coroutines, GeneratesPositiveNumbersStartingFromOne)
{
    auto start = 1;
    auto end = 1'000'000;
    auto gen = range<int>(start, end);
    for(int i = start; i <= end; i++)
    {
        ASSERT_TRUE(gen.next());
        ASSERT_EQ(gen.value(), i);
    }
}

TEST(Coroutines, GeneratorStepsCorrectly)
{
    auto start = 1;
    auto end = 100;
    auto step = 7;
    auto gen = range<int>(start, end, step);
    for(int i = start; i <= end; i += step)
    {
        ASSERT_TRUE(gen.next());
        ASSERT_EQ(gen.value(), i);
    }
}