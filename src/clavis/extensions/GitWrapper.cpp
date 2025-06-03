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
        try {
            auto pw = System::ProcessWrapper();
            pw.Init("git", {"rev-parse", "--is-inside-work-tree"}, path);

            return pw.GetExitCode() == 0;
        } catch (...) {
            std::cout << "Error checking if we have a git repo. Is git installed?";
            return false;
        }
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

        if (pw.GetExitCode() != 0) {
            std::string command = "git";
            for (const auto& arg : args)
                command += std::string(" ") + arg;
            std::cerr << "Error performing " + command + "\n";
            return false;
        }

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

    std::string FormatCommitMessage(const std::string message) {
#ifdef __WINDOWS__
        return std::string("\"" + message + "\"");
#elif defined __LINUX__
        return message;
#endif
    }

    void CommitNewFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"add", path.string()}) && PerformGitCommand({"commit", "-m", FormatCommitMessage(_(GIT_ADDED_PASSWORD_COMMIT_MESSAGE, name))});
    }
    void RemoveFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"rm", path.string()}) && PerformGitCommand({"commit", "-m", FormatCommitMessage(_(GIT_REMOVED_PASSWORD_COMMIT_MESSAGE, name))});
    }
    void RemoveFolder(const std::filesystem::path &path, const std::string &name) {
        PerformGitCommand({"rm", "-r", path.string()}) && PerformGitCommand({"commit", "-m", FormatCommitMessage(_(GIT_REMOVED_DIRECTORY_COMMIT_MESSAGE, name))});
    }
    void Move(const std::filesystem::path &from, const std::filesystem::path &to) {
        PerformGitCommand({"mv", from.string(), to.string()}) && PerformGitCommand({"commit", "-m", FormatCommitMessage(_(GIT_MOVED_ELEMENT_COMMIT_MESSAGE, from.string(), to.string()))});
    }



}