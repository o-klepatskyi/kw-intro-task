#include "log.h"
#include "util.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstddef> // size_t
#include <functional> // std::function
#include <filesystem>
#include <fstream>

namespace kw
{

    namespace fs = std::filesystem;

    struct RemoteDirEntry
    {
        std::string remotePath;
        size_t size = 0;
        bool isFile = false; // assume remote only has regular files or dirs

        RemoteDirEntry() noexcept = default;
        RemoteDirEntry(std::string path, size_t size, bool isFile) noexcept
            : remotePath{path}, size{size}, isFile{isFile} {}

        const char* path() const noexcept { return remotePath.c_str(); }
    };

    class FakeAsyncFTPExample
    {
        // spec: last LIST result needs to be kept around for the UI
        //       this complicates the implementation
        std::vector<RemoteDirEntry> listed;
        std::string listedPath;

    public:

        FakeAsyncFTPExample() noexcept = default;

        /** @returns List of remote dir entries from the last `listFiles` call, for the UI */
        const std::vector<RemoteDirEntry>& getListed() const noexcept { return listed; }

        /** @returns Listed remote path name from the last `listFiles` call, for the UI */
        const std::string& getListedPath() const noexcept { return listedPath; }

        /**
         * @brief Downloads the first file that matches the predicate
         * @param remotePath Remote path to fetch LIST of files from
         * @param predicate Files filter to select the file (convoluted extra step)
         * @param onProgress Progress report callback for the UI progress bar
         * @returns Local temp path of the downloaded file
         * 
         * TODO: return an async object instead of blocking here
         */
        std::string downloadFirstMatch(const std::string& remotePath, 
                                       std::function<bool(std::string_view)> predicate,
                                       std::function<void(int)> onProgress)
        {
            // Step 1. Get list of files. TODO: make async
            auto files = listFiles(remotePath);

            // Step 2. Find the first matching file. TODO: make async
            RemoteDirEntry match = findMatchingFile(files, std::move(predicate));

            // Step 3. Download the file. TODO: make async
            std::string tempPath = downloadFile(match, std::move(onProgress));

            return tempPath;
        }

    private:

        std::vector<RemoteDirEntry> listFiles(const std::string& remotePath)
        {
            LogInfo("LIST %s", remotePath.c_str());

            if (!fs::exists(remotePath)) // failures are handled by exceptions
                throw std::runtime_error{"FTP remote path does not exist: " + remotePath};

            std::vector<RemoteDirEntry> list;
            for (const fs::directory_entry& dirEntry : fs::directory_iterator{remotePath})
            {
                list.emplace_back(dirEntry.path().string(), dirEntry.file_size(), dirEntry.is_regular_file());
                const RemoteDirEntry& e = list.back();
                if (e.isFile) LogInfo("  file %s (%zu KB)", e.path(), e.size);
                else          LogInfo("  dir  %s", e.path());
            }

            listed = list; // make a copy for the UI to use later
            listedPath = remotePath;
            return list;
        }

        static RemoteDirEntry findMatchingFile(const std::vector<RemoteDirEntry>& list,
                                               std::function<bool(std::string_view)> predicate)
        {
            for (auto& e : list)
                if (e.isFile && predicate(e.remotePath))
                    return e;
            throw std::runtime_error{"FTP no files matched the search pattern"};
        }

        std::string downloadFile(const RemoteDirEntry& remoteFile,
                                 std::function<void(int)> onProgress)
        {
            LogInfo("DOWNLOAD %s (%zu KB)", remoteFile.path(), remoteFile.size / 1024);
            if (!remoteFile.isFile)
                throw std::runtime_error{"FTP download failed, not a file: " + remoteFile.remotePath};
            
            std::ifstream inFile { remoteFile.remotePath, std::ios::binary };
            if (!inFile)
                throw std::runtime_error{"FTP download request failed: " + remoteFile.remotePath};

            std::string tempPath = (fs::temp_directory_path() / fs::path{remoteFile.remotePath}.filename()).string();
            std::ofstream outFile { tempPath, std::ios::binary };
            if (!outFile)
                throw std::runtime_error{"FTP failed to create temp file at: " + tempPath};
            
            // perform a "fake download"
            int prevProgress = -1;
            char buf[128]; // artificially small buffer for this fake example
            for (size_t i = 0; i < remoteFile.size; ++i)
            {
                inFile.read(buf, sizeof(buf));
                size_t bytesRead = inFile.gcount();
                outFile.write(buf, bytesRead);

                i += bytesRead;

                // report progress to the UI
                if (int progress = static_cast<int>((i * 100) / remoteFile.size); prevProgress != progress)
                {
                    prevProgress = progress;
                    onProgress(progress); // the UI will handle synchronization
                }
            }
            return tempPath;
        }
    };
}



int main(int, char**)
{
	// usage example, errors must be handled
    // the actual work should be done in a background thread
    // UI holds something equivalent to a future<string> to get the final result
    try
    {
	    kw::FakeAsyncFTPExample ftp;
	    std::string file = ftp.downloadFirstMatch("/Projects",
			[](std::string_view f) { return f.ends_with(".txt"); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
    LogInfo("%s", getProjectPath().c_str());
    return 0;
}