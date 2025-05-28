#include <extensions/GitWrapper.h>

#include <system/ProcessWrapper.h>
#include <filesystem>
#include <iostream>

#include <settings/Settings.h>
#include <system/Extensions.h>


namespace Clavis::Git {
    std::filesystem::path GitPath() {
        return Settings::PASSWORD_STORE_PATH.GetValue();
    }

    bool IsGitRepo(const std::filesystem::path& path) {
        auto pw = System::ProcessWrapper();
        pw.Init("git", {"rev-parse", "--is-inside-work-tree"}, path);

        return pw.GetExitCode() == 0;
    }

    bool IsGitRepo() {
        return IsGitRepo(GitPath());
    }


    bool PerformGitCommand(std::vector<std::string> args) {
        const auto gitPath = GitPath();

        if (!IsGitRepo(gitPath))
            return false;

        auto pw = System::ProcessWrapper();
        pw.Init("git", args, gitPath);
        pw.Wait();

        if (pw.GetExitCode() != 0)
            return false;

        return true;
    }

    bool TryPull() {
        return PerformGitCommand({"pull"});
    }
    bool TryPush() {
        return PerformGitCommand({"push"});
    }
    bool TrySync() {
        return TryPull() && TryPush();
    }

    void CommitNewFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"add", path.string()});
        PerformGitCommand({"commit", "-m", _(GIT_ADDED_PASSWORD_COMMIT_MESSAGE, name)});
    }
    void RemoveFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"rm", path.string()});
        PerformGitCommand({"commit", "-m", _(GIT_REMOVED_PASSWORD_COMMIT_MESSAGE, name)});
    }
    void RemoveFolder(const std::filesystem::path &path, const std::string &name) {
        PerformGitCommand({"rm", "-r", path.string()});
        PerformGitCommand({"commit", "-m", _(GIT_REMOVED_DIRECTORY_COMMIT_MESSAGE, name)});

        // Git won't remove the folder if it's empty
        if (System::DirectoryExists(path))
            std::filesystem::remove_all(path);
    }
    void Move(const std::filesystem::path &from, const std::filesystem::path &to) {
        PerformGitCommand({"mv", from.string(), to.string()});
        PerformGitCommand({"commit", "-m", _(GIT_MOVED_ELEMENT_COMMIT_MESSAGE, from.string(), to.string())});
    }



}