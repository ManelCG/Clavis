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
            auto result = pw.GetExitCode() == 0;
            pw.Cleanup();
            return result;
        } catch (...) {
            std::cout << "Error checking if we have a git repo. Is git installed?";
            return false;
        }
    }

    bool IsGitRepo() {
        return IsGitRepo(GitPath());
    }


    // Run a git command in an arbitrary directory without requiring it to be a repo already.
    // Used for init, clone, and config operations where the target dir may not be a repo yet.
    static bool PerformGitCommandAt(const std::filesystem::path &workingDir, const std::vector<std::string> &args, std::string* output = nullptr, bool writeErrorMessage = true) {
        try {
            auto pw = System::ProcessWrapper();
            pw.Init("git", args, workingDir);
            pw.Wait();

            const int code = pw.GetExitCode();
            if (output != nullptr)
                *output = pw.GetOutput();
            pw.Cleanup();

            if (code != 0) {
                if (writeErrorMessage) {
                    std::string command = "git";
                    for (const auto& arg : args)
                        command += std::string(" ") + arg;
                    std::cerr << "Error performing " + command + "\n";
                }
                return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    // DirectoryExists is a cheap fast-fail; git reports its own error if the dir isn't a repo.
    static bool PerformGitCommand(const std::vector<std::string> &args, std::string* output = nullptr, bool writeErrorMessage = true) {
        const auto gitPath = GitPath();
        if (!System::DirectoryExists(gitPath))
            return false;
        return PerformGitCommandAt(gitPath, args, output, writeErrorMessage);
    }

    // Windows: wrap in quotes and escape any embedded double-quotes to avoid command-line parsing failures.
    static std::string FormatStringArgument(const std::string &message) {
#ifdef __WINDOWS__
        std::string escaped = message;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.insert(pos, "\\");
            pos += 2;
        }
        return "\"" + escaped + "\"";
#elif defined __UNIX__
        return message;
#endif
    }

    // -X theirs auto-resolves conflicts in favour of the remote. This is correct for encrypted
    // binary files (.gpg) where no line-level merge is possible. Using merge (not rebase) keeps
    // history linear-friendly and means push needs no force flag.
    bool TryPull() {
        return PerformGitCommand({"pull", "-X", "theirs"});
    }

    bool TryPush() {
        return PerformGitCommand({"push"});
    }

    bool TrySync() {
        return TryPull() && TryPush();
    }

    // Run from parent_path() so git creates the destination directory itself.
    bool TryCloneRepo(const std::filesystem::path &destination, const std::string &upstreamURL) {
        return PerformGitCommandAt(destination.parent_path(), {"clone", upstreamURL, destination.string()});
    }

    // Run inside destination so both init and remote-add target the right repo, not GitPath().
    bool TryInitRepo(const std::filesystem::path &destination, const std::string &upstreamURL) {
        return PerformGitCommandAt(destination, {"init"}) &&
               PerformGitCommandAt(destination, {"remote", "add", "origin", FormatStringArgument(upstreamURL)});
    }

    bool TryGetConfig(const std::string &config, bool global, std::string &out) {
        auto flag = global? "--global" : "--local";
        if (!PerformGitCommand({"config", flag, config}, &out, false))
            return false;
        out = StringHelper::TrimTrailingNewlines(out);
        return true;
    }

    bool TryGetConfigLocalGlobal(const std::string &config, std::string &out) {
        if (TryGetConfig(config, false, out))
            return true;
        return TryGetConfig(config, true, out);
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

    // Path-taking overloads are needed during first-run: at that point GitPath() still returns
    // the default store location, not the path the user just initialised or cloned into.
    bool TrySetUsername(const std::filesystem::path &repoPath, const std::string &username) {
        return PerformGitCommandAt(repoPath, {"config", "--local", "user.name", FormatStringArgument(username)});
    }
    bool TrySetEmail(const std::filesystem::path &repoPath, const std::string &email) {
        return PerformGitCommandAt(repoPath, {"config", "--local", "user.email", FormatStringArgument(email)});
    }


    bool CommitNewFile(const std::filesystem::path& path, const std::string& name) {
        return PerformGitCommand({"add", path.string()}) &&
               PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_ADDED_PASSWORD_COMMIT_MESSAGE, name))});
    }

    void CommitImport(const std::vector<std::filesystem::path>& paths, const std::string& name) {
        if (paths.empty()) return;
        std::vector<std::string> addArgs = {"add"};
        for (const auto& p : paths)
            addArgs.push_back(p.string());
        if (PerformGitCommand(addArgs))
            PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_IMPORTED_PASSWORD_STORE_COMMIT_MESSAGE, name))});
    }

    bool RemoveFile(const std::filesystem::path& path, const std::string& name) {
        if (!PerformGitCommand({"rm", path.string()})) return false;
        return PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_REMOVED_PASSWORD_COMMIT_MESSAGE, name))});
    }
    bool RemoveFolder(const std::filesystem::path &path, const std::string &name) {
        if (!PerformGitCommand({"rm", "-r", path.string()})) return false;
        return PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_REMOVED_DIRECTORY_COMMIT_MESSAGE, name))});
    }

    bool Move(const std::filesystem::path &from, const std::filesystem::path &to) {
        return PerformGitCommand({"mv", from.string(), to.string()}) &&
               PerformGitCommand({"commit", "-m", FormatStringArgument(_(GIT_MOVED_ELEMENT_COMMIT_MESSAGE, from.string(), to.string()))});
    }
}
