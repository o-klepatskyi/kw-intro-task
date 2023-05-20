#pragma once

#include <future>
#include <thread>
#include <atomic>
#include <chrono>

template<typename TaskType>
inline void useThreads( const int nThreads,
                        std::promise<void>&& promise,
                        TaskType task)
{
    std::vector<std::thread> threads;
    threads.reserve(nThreads);

    for (int j = 0; j < nThreads; j++)
    {
        threads.emplace_back(task);
    }

    promise.set_value(); // start all threads simultaneously

    for(auto& t : threads)
    {
        t.join();
    }
};

// the same with std::async
template<typename TaskType>
inline void useAsync(   const int nThreads,
                        std::promise<void>&& promise,
                        TaskType task)
{
    std::vector<std::future<void>> futures;
    futures.reserve(nThreads);

    for (int j = 0; j < nThreads; j++)
    {
        futures.emplace_back(std::async(std::launch::async, task));
    }

    promise.set_value(); // start all threads simultaneously

    for(auto& f : futures)
    {
        f.get();
    }
}

int multithreadVolatileIncrement(const int nThreads)
{
    volatile int i = 0;
    std::promise<void> p;
    auto startFuture = p.get_future().share();
    auto waitAndIncrement = [&i, startFuture] {
        startFuture.wait();
        ++i;
    };
    useThreads(nThreads, std::move(p), waitAndIncrement);
    return i;
}

int multithreadAtomicIncrement(const int nThreads)
{
    std::atomic<int> i{0};
    std::promise<void> p;
    auto startFuture = p.get_future().share();

    auto waitAndIncrement = [&i, startFuture] {
        startFuture.wait();
        int randomTime = (std::rand() % 10) + 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(randomTime));
        ++i;
    };
    useThreads(nThreads, std::move(p), waitAndIncrement);
    return i;
}

int multithreadVolatileIncrementWithAsync(const int nThreads)
{
    volatile int i = 0;
    std::promise<void> p;
    auto startFuture = p.get_future().share();
    auto waitAndIncrement = [&i, startFuture] {
        startFuture.wait();
        int randomTime = (std::rand() % 10) + 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(randomTime));
        ++i;
    };
    useAsync(nThreads, std::move(p), waitAndIncrement);
    return i;
}

int multithreadAtomicIncrementWithAsync(const int nThreads)
{
    std::atomic<int> i{0};
    std::promise<void> p;
    auto startFuture = p.get_future().share();

    auto waitAndIncrement = [&i, startFuture] {
        startFuture.wait();
        ++i;
    };
    useAsync(nThreads, std::move(p), waitAndIncrement);
    return i;
}

template <typename Duration, typename T>
inline long long elapsedTime(T startTime) noexcept
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<Duration>(currentTime - startTime);
    return elapsed.count();
};

template<typename T>
inline auto elapsedTimeMs(T startTime) noexcept
{
    return elapsedTime<std::chrono::milliseconds>(startTime);
};

template<typename T>
inline auto elapsedTimeNano(T startTime) noexcept
{
    return elapsedTime<std::chrono::nanoseconds>(startTime);
};

std::pair<long long, long long> asyncMeasureTime(std::launch launch)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto task = [startTime] {
        return elapsedTimeNano(startTime);
    };
    auto timeElapsedF = std::async(launch, task);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto afterAsync = elapsedTimeNano(startTime);
    return { afterAsync, timeElapsedF.get() };
}