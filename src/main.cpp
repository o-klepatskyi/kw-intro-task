#include "util.h"
#include "FtpExampleSync.h"
#include "FtpExampleAsync.h"
#include "FtpExampleCoro.h"
#include "future_coro.h"

#include <thread>
#include <stdexcept>

void useSync(const std::string& path, const std::string& pattern)
{
    LogInfo("======== Using sync");
    try
    {
	    kw::FTPExampleSync ftp;
	    std::string file = ftp.downloadFirstMatch(path,
			[&pattern] (std::string_view f) { return f.ends_with(pattern); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
}

void useAsync(const std::string& path, const std::string& pattern)
{
    LogInfo("======== Using async");
    try
    {
	    kw::FTPExampleAsync ftp;
	    auto fileF = ftp.downloadFirstMatch(path,
			[&pattern](std::string_view f) { return f.ends_with(pattern); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        LogInfo("Doing my job...");
        std::string file = fileF.get();
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
}

std::future<void> useCoro(const std::string& path, const std::string& pattern) noexcept
{
    LogInfo("======== Using coroutines");
    LogInfo("Current thread ID: %llu", std::this_thread::get_id());
    try
    {
	    kw::FTPExampleCoro ftp;
	    auto file = co_await ftp.downloadFirstMatch(path,
			[&pattern](std::string_view f) { return f.ends_with(pattern); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
    co_return;
}

std::future<int> coro1(int x)
{
    LogInfo("Current thread ID: %llu", std::this_thread::get_id());
    co_return x * 2;
}

std::future<int> coro2()
{
    LogInfo("Current thread ID on start: %llu", std::this_thread::get_id());
    int result = co_await coro1(5);
    LogInfo("Current thread ID after co_await: %llu", std::this_thread::get_id());
    result++;
    co_return result;
}

std::future<void> throws()
{
    LogInfo("Current thread ID on start: %llu", std::this_thread::get_id());
    throw std::exception {};
    co_return;
}

std::future<int> usingLambda()
{
    LogInfo("Current thread ID on start: %llu", std::this_thread::get_id());
    int x = co_await [] {
        LogInfo("Current thread ID on start: %llu", std::this_thread::get_id());
        return 5;
    };
    LogInfo("Current thread ID after co_await: %llu", std::this_thread::get_id());
    co_return x * 2;
}

std::future<int> usingLambdaThrows()
{
    LogInfo("Current thread ID on start: %llu", std::this_thread::get_id());
    int x = co_await [] () -> std::future<int> {
        LogInfo("Current thread ID on start in lambda: %llu", std::this_thread::get_id());
        throw std::exception {};
        co_return 5;
    }();
    LogInfo("Current thread ID after co_await: %llu", std::this_thread::get_id());
    co_return x * 2;
}

int main(int, char**)
{
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";

    useCoro(path, pattern).wait();
    
    // useSync(path, pattern);
    // useAsync(path, pattern);
    // useCoro(path, pattern);

    // LogInfo("Main thread ID: %llu", std::this_thread::get_id());
    // auto f = coro2();
    // LogInfo("%u", f.get());
    // auto f2 = throws();
    // try
    // {
    //     f2.get();
    // }
    // catch(...)
    // {
    //     LogError("Cathed error!");
    // }
    // try
    // {
    //     LogInfo("%u", usingLambdaThrows().get());
    // }
    // catch(...)
    // {
    //     LogError("Cathed error!");
    // }

    
    return 0;
}