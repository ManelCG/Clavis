#include <extensions/GitWrapper.h>

#include <system/ProcessWrapper.h>
#include <filesystem>
#include <iostream>

#include <settings/Settings.h>
#include <system/Extensions.h>

#include "extensions/StringHelper.h"


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


    bool PerformGitCommand(const std::vector<std::string> &args, std::string* output = nullptr, bool writeErrorMessage = true) {
        const auto gitPath = GitPath();

        if (!IsGitRepo(gitPath))
            return false;

        auto pw = System::ProcessWrapper();
        pw.Init("git", args, gitPath);
        pw.Wait();

        if (pw.GetExitCode() != 0) {
            if (writeErrorMessage) {
                std::string command = "git";
                for (const auto& arg : args)
                    command += std::string(" ") + arg;
                std::cerr << "Error performing " + command + "\n";
            }
            return false;
        }

        if (output != nullptr)
            *output = pw.GetOutput();

        return true;
    }

    std::string FormatStringArgument(const std::string &message) {
#ifdef __WINDOWS__
        return std::string("\"" + message + "\"");
#elif defined __LINUX__
        return message;
#endif
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
    bool TryCloneRepo(const std::filesystem::path &destination, const std::string &upstreamURL) {
        return PerformGitCommand({"clone", upstreamURL, destination.string()} );
    }
    bool TryInitRepo(const std::filesystem::path &destination, const std::string &upstreamURL) {
        return
            PerformGitCommand({"init", destination}) &&
            PerformGitCommand({"remote", "add", "origin", FormatStringArgument(upstreamURL)});
    }

    bool TryGetConfig(const std::string &config, bool global, std::string &out) {
        std::string ret;

        auto flag = global? "--global" : "--local";

        if (! PerformGitCommand({"config", flag, config}, &out, false))
            return false;

        out = StringHelper::TrimTrailingNewlines(out);
        return true;
    }
    bool TryGetConfigLocalGlobal(const std::string &config, std::string &out) {
        if (TryGetConfig(config, false, out))
            return true;

        if (TryGetConfig(config, true, out))
            return true;

        return false;
    }

    bool TryGetUsername(std::string &outUsername) {
        return TryGetConfigLocalGlobal("user.name", outUsername);
    }

    bool TryGetEmail(std::string &outEmail) {
        return TryGetConfigLocalGlobal("user.email", outEmail);
    }

    bool TrySetUsername(const std::string &username) {
        return PerformGitCommand({"config", "--local", "user.name", FormatStringArgument(username)});
    }

    bool TrySetEmail(const std::string &email) {
        return PerformGitCommand({"config", "--local", "user.email", FormatStringArgument(email)});
    }


    void CommitNewFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"add", path.string()}) && PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_ADDED_PASSWORD_COMMIT_MESSAGE, name))});
    }
    void RemoveFile(const std::filesystem::path& path, const std::string& name) {
        PerformGitCommand({"rm", path.string()}) && PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_REMOVED_PASSWORD_COMMIT_MESSAGE, name))});
    }
    void RemoveFolder(const std::filesystem::path &path, const std::string &name) {
        PerformGitCommand({"rm", "-r", path.string()}) && PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_REMOVED_DIRECTORY_COMMIT_MESSAGE, name))});
    }
    void Move(const std::filesystem::path &from, const std::filesystem::path &to) {
        PerformGitCommand({"mv", from.string(), to.string()}) && PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_MOVED_ELEMENT_COMMIT_MESSAGE, from.string(), to.string()))});
    }



}
