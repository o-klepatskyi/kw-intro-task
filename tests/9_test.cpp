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

    auto rStream = readStream(stream);
    auto firstInput  = rStream.readLine();
    EXPECT_STREQ(first.c_str(), firstInput.c_str());
    auto secondInput = rStream.readLine();
    EXPECT_STREQ(second.c_str(), secondInput.c_str());
    auto thirdInput  = rStream.readLine();
    EXPECT_STREQ(third.c_str(), thirdInput.c_str());
}