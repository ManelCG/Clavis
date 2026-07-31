#pragma once
#include <filesystem>
#include <vector>

namespace Clavis::Git {
    bool IsGitRepo();
    bool IsGitRepo(const std::filesystem::path& path);

    bool TryPull();
    bool TryPush();
    bool TrySync();

    bool TryCloneRepo(const std::filesystem::path& destination, const std::string& upstreamURL);
    bool TryInitRepo(const std::filesystem::path& destination, const std::string& upstreamURL);

    bool TryGetConfig(const std::string& config, bool global, std::string& out);
    bool TryGetConfigLocalGlobal(const std::string& config, std::string& out);

    bool TryGetUsername(std::string& outUsername);
    bool TryGetEmail(std::string& outEmail);
    bool TrySetUsername(const std::string& username);
    bool TrySetEmail(const std::string& email);
    bool TrySetUsername(const std::filesystem::path& repoPath, const std::string& username);
    bool TrySetEmail(const std::filesystem::path& repoPath, const std::string& email);

    // Stages a single file and commits it with the given message. Also covers edits, since
    // `git add` stages modifications too.
    bool CommitFile(const std::filesystem::path& path, const std::string& message);

    bool CommitNewFile(const std::filesystem::path& path, const std::string& name);
    void CommitImport(const std::vector<std::filesystem::path>& paths, const std::string& name);
    bool RemoveFile(const std::filesystem::path& path, const std::string& name);
    bool RemoveFolder(const std::filesystem::path& path, const std::string& name);
    // Set overwrite when the destination is known to exist and the user has confirmed replacing
    // it; `git mv` fails on an existing destination otherwise.
    bool Move(const std::filesystem::path& from, const std::filesystem::path& to, bool overwrite = false);
}
