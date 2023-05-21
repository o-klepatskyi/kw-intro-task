#include "9_coroutines.h"
#include "gtest/gtest.h"
#include "MemoryLeakDetector.h"

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