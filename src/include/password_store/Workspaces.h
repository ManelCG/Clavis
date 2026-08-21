#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <password_store/PasswordStoreElement.h>

namespace Clavis::Workspaces {
    // The workspace database lives in a single hidden file at the root of the password store.
    // It is deliberately not encrypted: it holds no secrets, only names and locations, and being
    // plain JSON means it survives being merged by hand after a git conflict.
    constexpr const char* WORKSPACES_FILENAME = ".workspaces";

    // One pointer from a workspace to a real file in the store. The name is the workspace's own
    // label for that file -- the whole point of a workspace -- so the same file can read as
    // "Gmail work" in one and "personal mail" in another.
    struct WorkspaceEntry {
        std::string name;
        std::filesystem::path target;   // store-relative, always with '/' separators on disk
    };

    // A named, curated list of files that behaves like a folder without being one. `path` is the
    // store-relative directory the workspace shows up in; an empty path means the store root.
    struct Workspace {
        std::string name;
        std::filesystem::path path;
        std::vector<WorkspaceEntry> entries;
    };

    // Turns a workspace's entries into folderview elements. Targets that no longer exist on disk
    // come back flagged as missing rather than being dropped, so a file that is temporarily away
    // (a half-finished pull, a manual move) is never silently forgotten.
    std::vector<PasswordStoreElements::PasswordStoreElement> BuildElements(
        const Workspace& workspace, const std::filesystem::path& storeRoot);

    class WorkspaceDB {
    public:
        WorkspaceDB() = default;

        // A missing or unreadable file yields an empty database. A store without workspaces is a
        // perfectly normal store, so this is never an error.
        static WorkspaceDB Load(const std::filesystem::path& storeRoot);

        // Writes the file and, in a git store, commits it under the given message. Only ever
        // called after a real change, so an untouched store never grows an empty commit. The
        // message is the caller's to phrase: only the caller knows what actually happened, and a
        // workspace's history is far more use when it says so.
        bool Save(const std::string& commitMessage) const;

        [[nodiscard]] const std::vector<Workspace>& GetAll() const;
        [[nodiscard]] std::vector<Workspace> GetIn(const std::filesystem::path& relDir) const;
        bool TryGet(const std::filesystem::path& relDir, const std::string& name, Workspace& out) const;

        [[nodiscard]] bool Exists(const std::filesystem::path& relDir, const std::string& name) const;

        // Keyed by (path, name): replaces the workspace at that location, or appends a new one.
        void Upsert(const Workspace& workspace);
        bool Remove(const std::filesystem::path& relDir, const std::string& name);
        bool Move(const std::filesystem::path& oldDir, const std::string& oldName,
                  const std::filesystem::path& newDir, const std::string& newName);

        bool AddEntry(const std::filesystem::path& relDir, const std::string& name,
                      const WorkspaceEntry& entry);
        bool RemoveEntry(const std::filesystem::path& relDir, const std::string& name,
                         const std::filesystem::path& target);
        bool RenameEntry(const std::filesystem::path& relDir, const std::string& name,
                         const std::filesystem::path& target, const std::string& newEntryName);

        // Keeping workspaces pointing at the right files when the store moves underneath them.
        // Both of these match an exact target and anything nested under it, so renaming or
        // deleting a whole folder fixes up the entries inside it too.
        bool RetargetElement(const std::filesystem::path& oldRel, const std::filesystem::path& newRel);
        bool RemoveElementEverywhere(const std::filesystem::path& rel);
        [[nodiscard]] std::vector<std::string> FindWorkspacesContaining(const std::filesystem::path& rel) const;

        [[nodiscard]] std::filesystem::path GetRoot() const;

    private:
        explicit WorkspaceDB(std::filesystem::path storeRoot);

        Workspace* Find(const std::filesystem::path& relDir, const std::string& name);
        [[nodiscard]] const Workspace* Find(const std::filesystem::path& relDir, const std::string& name) const;

        std::filesystem::path root;
        std::vector<Workspace> workspaces;
    };

    // Store-relative paths are normalised to a single canonical form -- forward slashes, no "."
    // for the root -- so that a workspace written on Windows resolves on Linux and vice versa.
    std::filesystem::path NormalizeRelative(const std::filesystem::path& p);
    std::string ToStorageString(const std::filesystem::path& p);

    // True when `rel` is `ancestor` itself or sits underneath it.
    bool IsSameOrUnder(const std::filesystem::path& rel, const std::filesystem::path& ancestor);
}
