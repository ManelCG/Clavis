#pragma once
#include <filesystem>

namespace Clavis::Git {
    bool IsGitRepo();
    bool IsGitRepo(const std::filesystem::path& path);

    bool TryPull();
    bool TryPush();
    bool TrySync();

    void CommitNewFile(const std::filesystem::path& path, const std::string& name);
    void RemoveFile(const std::filesystem::path& path, const std::string& name);
    void RemoveFolder(const std::filesystem::path& path, const std::string& name);
    void Move(const std::filesystem::path& from, const std::filesystem::path& to);
}
