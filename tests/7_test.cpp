#include "7_the_concurrency_api.h"
#include "gtest/gtest.h"

#include <system_error>
#include <future>

TEST(C7_concurrency, VolatileIsNotThreadSafe)
{
    const int nThreads = 100;
    try
    {
        int result = multithreadVolatileIncrement(nThreads);
        EXPECT_NE(result, nThreads) << "For small number of threads (N<1000) this can work out.";
    }
    catch(const std::system_error& err)
    {
        FAIL() << "Error while creating threads:\n" << err.what();
    }
}

TEST(C7_concurrency, AtomicIsThreadSafe)
{
    const int nThreads = 1000;
    try
    {
        int result = multithreadAtomicIncrement(nThreads);
        EXPECT_EQ(result, nThreads);
    }
    catch(const std::system_error& err)
    {
        FAIL() << "Error while creating threads:\n" << err.what();
    }
}

TEST(C7_concurrency, VolatileIsNotThreadSafeWithAsync)
{
    const int nThreads = 100;
    try
    {
        int result = multithreadVolatileIncrementWithAsync(nThreads);
        EXPECT_NE(result, nThreads) << "For small number of threads (N<1000) this can work out.";
    }
    catch(const std::system_error& err)
    {
        FAIL() << "Error while creating threads:\n" << err.what();
    }
}

TEST(C7_concurrency, AtomicIsThreadSafeWithAsync)
{
    const int nThreads = 1000;
    try
    {
        int result = multithreadAtomicIncrementWithAsync(nThreads);
        EXPECT_EQ(result, nThreads);
    }
    catch(const std::system_error& err)
    {
        FAIL() << "Error while creating threads:\n" << err.what();
    }
}

TEST(C7_concurrency, DefferredLaunchIsAlwaysLaunchedInGet)
{
    auto pair = asyncMeasureTime(std::launch::deferred);
    EXPECT_LE(pair.first, pair.second);
}

TEST(C7_concurrency, AsyncLaunchIsAlwaysLaunchedInOtherThread)
{
    auto pair = asyncMeasureTime(std::launch::async);
    EXPECT_GE(pair.first, pair.second);
}