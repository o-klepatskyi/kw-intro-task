#include "util.h"
#include "FtpExampleSync.h"
#include "FtpExampleAsync.h"
#include "FtpExampleCoro.h"
#include "SyncWaitTask.h"
#include <thread>

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

void useCoro(const std::string& path, const std::string& pattern)
{
    LogInfo("======== Using coroutines");
    try
    {
	    kw::FTPExampleCoro ftp;
	    auto fileTask = ftp.downloadFirstMatch(path,
			[&pattern](std::string_view f) { return f.ends_with(pattern); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        auto waitTask = startTaskAsync(std::move(fileTask));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        LogInfo("Doing my job...");
        std::string file = waitTask.get().result();
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
}

int main(int, char**)
{
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";
    useSync(path, pattern);
    useAsync(path, pattern);
    useCoro(path, pattern);

    return 0;
}