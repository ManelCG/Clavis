#include <password_store/Workspaces.h>

#include <algorithm>
#include <map>

#include <extensions/GitWrapper.h>
#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::Workspaces {
    std::filesystem::path NormalizeRelative(const std::filesystem::path& p) {
        auto normalized = p.lexically_normal();

        // lexically_normal turns an empty path into "." for a directory; the root of the store is
        // stored as an empty path so that appending to it never produces a stray "./".
        if (normalized == ".")
            return {};

        // A trailing separator would make otherwise-equal paths compare unequal.
        auto s = normalized.generic_string();
        while (s.size() > 1 && s.back() == '/')
            s.pop_back();

        if (s == ".")
            return {};

        return std::filesystem::path(s);
    }

    std::string ToStorageString(const std::filesystem::path& p) {
        return NormalizeRelative(p).generic_string();
    }

    bool IsSameOrUnder(const std::filesystem::path& rel, const std::filesystem::path& ancestor) {
        const auto a = ToStorageString(ancestor);
        const auto r = ToStorageString(rel);

        if (a.empty())
            return true;
        if (r == a)
            return true;

        // Compare on a separator boundary, or "P/ab" would count as living under "P/a".
        return r.size() > a.size() && r.compare(0, a.size(), a) == 0 && r[a.size()] == '/';
    }

    std::vector<PasswordStoreElements::PasswordStoreElement> BuildElements(
        const Workspace& workspace, const std::filesystem::path& storeRoot)
    {
        // An entry's own name is what the user sees, so a workspace holding two files that ended
        // up with the same label would be unreadable. When that happens both fall back to their
        // store-relative path, which is always unique.
        std::map<std::string, int> nameCounts;
        for (const auto& entry : workspace.entries)
            nameCounts[entry.name]++;

        std::vector<PasswordStoreElements::PasswordStoreElement> ret;
        ret.reserve(workspace.entries.size());

        for (const auto& entry : workspace.entries) {
            const auto rel = NormalizeRelative(entry.target);
            const auto fullpath = storeRoot / rel;

            auto elem = System::FileExists(fullpath)
                ? PasswordStoreElements::PasswordStoreElement(fullpath)
                : PasswordStoreElements::PasswordStoreElement::MakeMissing(fullpath);

            const bool isAmbiguous = nameCounts[entry.name] > 1;
            elem.SetDisplayName(isAmbiguous ? rel.generic_string() : entry.name);

            ret.push_back(elem);
        }

        // Same grouping the rest of the folderview uses -- passwords, then 2FA, then anything
        // else -- but tie-broken on the displayed name rather than the on-disk filename, since
        // inside a workspace the two have nothing to do with each other.
        std::sort(ret.begin(), ret.end(), [](const auto& a, const auto& b) {
            if (a.GetType() != b.GetType())
                return static_cast<unsigned int>(a.GetType()) < static_cast<unsigned int>(b.GetType());
            return a.GetName() < b.GetName();
        });

        return ret;
    }

    WorkspaceDB::WorkspaceDB(std::filesystem::path storeRoot) : root(std::move(storeRoot)) {}

    std::filesystem::path WorkspaceDB::GetRoot() const {
        return root;
    }

    WorkspaceDB WorkspaceDB::Load(const std::filesystem::path& storeRoot) {
        WorkspaceDB db(storeRoot);

        const auto file = storeRoot / WORKSPACES_FILENAME;
        if (!System::FileExists(file))
            return db;

        std::string contents;
        if (!System::TryReadFile(file, contents))
            return db;

        // A malformed file must not take the whole store down with it: workspaces are a
        // convenience layer, and every password is still reachable through the tree without them.
        json parsed;
        try {
            parsed = json::parse(contents);
        } catch (...) {
            return db;
        }

        if (!parsed.is_object() || !parsed.contains("workspaces") || !parsed["workspaces"].is_array())
            return db;

        for (const auto& node : parsed["workspaces"]) {
            if (!node.is_object() || !node.contains("name"))
                continue;

            Workspace workspace;
            workspace.name = node["name"].get<std::string>();
            if (workspace.name.empty())
                continue;

            if (node.contains("path"))
                workspace.path = NormalizeRelative(std::filesystem::path(node["path"].get<std::string>()));

            if (node.contains("entries") && node["entries"].is_array()) {
                for (const auto& entryNode : node["entries"]) {
                    if (!entryNode.is_object() || !entryNode.contains("target"))
                        continue;

                    WorkspaceEntry entry;
                    entry.target = NormalizeRelative(std::filesystem::path(entryNode["target"].get<std::string>()));
                    if (entry.target.empty())
                        continue;

                    entry.name = entryNode.contains("name") ? entryNode["name"].get<std::string>() : "";
                    if (entry.name.empty())
                        entry.name = entry.target.filename().string();

                    workspace.entries.push_back(entry);
                }
            }

            db.workspaces.push_back(workspace);
        }

        return db;
    }

    bool WorkspaceDB::Save(const std::string& commitMessage) const {
        json out;
        out["version"] = 1;
        out["workspaces"] = json::array();

        for (const auto& workspace : workspaces) {
            json node;
            node["name"] = workspace.name;
            node["path"] = ToStorageString(workspace.path);
            node["entries"] = json::array();

            for (const auto& entry : workspace.entries) {
                json entryNode;
                entryNode["name"] = entry.name;
                entryNode["target"] = ToStorageString(entry.target);
                node["entries"].push_back(entryNode);
            }

            out["workspaces"].push_back(node);
        }

        const auto file = root / WORKSPACES_FILENAME;
        if (!System::TryWriteFile(file, out.dump(4)))
            return false;

        // Workspaces travel with the store, so they belong in the same history as the passwords
        // they point at. Failing to commit is not fatal -- the file is written either way.
        if (Git::IsGitRepo())
            Git::CommitFile(WORKSPACES_FILENAME, commitMessage);

        return true;
    }

    const std::vector<Workspace>& WorkspaceDB::GetAll() const {
        return workspaces;
    }

    std::vector<Workspace> WorkspaceDB::GetIn(const std::filesystem::path& relDir) const {
        const auto dir = ToStorageString(relDir);

        std::vector<Workspace> ret;
        for (const auto& workspace : workspaces)
            if (ToStorageString(workspace.path) == dir)
                ret.push_back(workspace);

        return ret;
    }

    const Workspace* WorkspaceDB::Find(const std::filesystem::path& relDir, const std::string& name) const {
        const auto dir = ToStorageString(relDir);

        for (const auto& workspace : workspaces)
            if (workspace.name == name && ToStorageString(workspace.path) == dir)
                return &workspace;

        return nullptr;
    }

    Workspace* WorkspaceDB::Find(const std::filesystem::path& relDir, const std::string& name) {
        return const_cast<Workspace*>(static_cast<const WorkspaceDB*>(this)->Find(relDir, name));
    }

    bool WorkspaceDB::TryGet(const std::filesystem::path& relDir, const std::string& name, Workspace& out) const {
        const auto* found = Find(relDir, name);
        if (found == nullptr)
            return false;

        out = *found;
        return true;
    }

    bool WorkspaceDB::Exists(const std::filesystem::path& relDir, const std::string& name) const {
        return Find(relDir, name) != nullptr;
    }

    void WorkspaceDB::Upsert(const Workspace& workspace) {
        auto normalized = workspace;
        normalized.path = NormalizeRelative(workspace.path);

        if (auto* existing = Find(normalized.path, normalized.name)) {
            *existing = normalized;
            return;
        }

        workspaces.push_back(normalized);
    }

    bool WorkspaceDB::Remove(const std::filesystem::path& relDir, const std::string& name) {
        const auto dir = ToStorageString(relDir);

        const auto it = std::find_if(workspaces.begin(), workspaces.end(), [&](const Workspace& w) {
            return w.name == name && ToStorageString(w.path) == dir;
        });

        if (it == workspaces.end())
            return false;

        workspaces.erase(it);
        return true;
    }

    bool WorkspaceDB::Move(const std::filesystem::path& oldDir, const std::string& oldName,
                           const std::filesystem::path& newDir, const std::string& newName) {
        auto* existing = Find(oldDir, oldName);
        if (existing == nullptr)
            return false;

        existing->path = NormalizeRelative(newDir);
        existing->name = newName;
        return true;
    }

    bool WorkspaceDB::AddEntry(const std::filesystem::path& relDir, const std::string& name,
                               const WorkspaceEntry& entry) {
        auto* workspace = Find(relDir, name);
        if (workspace == nullptr)
            return false;

        WorkspaceEntry normalized = entry;
        normalized.target = NormalizeRelative(entry.target);

        for (const auto& existing : workspace->entries)
            if (ToStorageString(existing.target) == ToStorageString(normalized.target))
                return false;

        workspace->entries.push_back(normalized);
        return true;
    }

    bool WorkspaceDB::RemoveEntry(const std::filesystem::path& relDir, const std::string& name,
                                  const std::filesystem::path& target) {
        auto* workspace = Find(relDir, name);
        if (workspace == nullptr)
            return false;

        const auto key = ToStorageString(target);
        const auto it = std::find_if(workspace->entries.begin(), workspace->entries.end(),
            [&](const WorkspaceEntry& e) { return ToStorageString(e.target) == key; });

        if (it == workspace->entries.end())
            return false;

        workspace->entries.erase(it);
        return true;
    }

    bool WorkspaceDB::RenameEntry(const std::filesystem::path& relDir, const std::string& name,
                                  const std::filesystem::path& target, const std::string& newEntryName) {
        auto* workspace = Find(relDir, name);
        if (workspace == nullptr)
            return false;

        const auto key = ToStorageString(target);
        for (auto& entry : workspace->entries) {
            if (ToStorageString(entry.target) != key)
                continue;

            if (entry.name == newEntryName)
                return false;

            entry.name = newEntryName;
            return true;
        }

        return false;
    }

    bool WorkspaceDB::RetargetElement(const std::filesystem::path& oldRel, const std::filesystem::path& newRel) {
        const auto oldKey = ToStorageString(oldRel);
        const auto newKey = ToStorageString(newRel);

        if (oldKey.empty() || oldKey == newKey)
            return false;

        bool changed = false;

        for (auto& workspace : workspaces) {
            for (auto& entry : workspace.entries) {
                const auto target = ToStorageString(entry.target);

                if (target == oldKey) {
                    entry.target = std::filesystem::path(newKey);
                    changed = true;
                    continue;
                }

                // A renamed folder takes everything below it along; the entries pointing inside
                // it have to follow, or the whole subtree of a workspace breaks at once.
                if (IsSameOrUnder(target, oldKey)) {
                    entry.target = std::filesystem::path(newKey + target.substr(oldKey.size()));
                    changed = true;
                }
            }
        }

        return changed;
    }

    bool WorkspaceDB::RemoveElementEverywhere(const std::filesystem::path& rel) {
        const auto key = ToStorageString(rel);
        if (key.empty())
            return false;

        bool changed = false;

        for (auto& workspace : workspaces) {
            const auto before = workspace.entries.size();

            workspace.entries.erase(
                std::remove_if(workspace.entries.begin(), workspace.entries.end(),
                    [&](const WorkspaceEntry& e) { return IsSameOrUnder(e.target, key); }),
                workspace.entries.end());

            if (workspace.entries.size() != before)
                changed = true;
        }

        return changed;
    }

    std::vector<std::string> WorkspaceDB::FindWorkspacesContaining(const std::filesystem::path& rel) const {
        const auto key = ToStorageString(rel);

        std::vector<std::string> ret;
        if (key.empty())
            return ret;

        for (const auto& workspace : workspaces) {
            for (const auto& entry : workspace.entries) {
                if (!IsSameOrUnder(entry.target, key))
                    continue;

                ret.push_back(workspace.name);
                break;
            }
        }

        return ret;
    }
}
