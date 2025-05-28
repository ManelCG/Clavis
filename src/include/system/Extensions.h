#pragma once

#include <string>

#include <filesystem>
#include <tools/json.hpp>
using json = nlohmann::json;

namespace Clavis::System {
    bool CopyToClipboard(const std::string& text);

    std::filesystem::path GetHomeFolder();
    std::filesystem::path GetConfigFolder();
    std::filesystem::path GetClavisConfigFolder();
    std::filesystem::path GetPasswordStoreDefaultFolder();
    std::filesystem::path GetPasswordStoreFolder();

#ifdef __WINDOWS__
    std::filesystem::path GetAppDataFolder();
#endif

    std::filesystem::path GetGPGIDPath();

    bool DirectoryExists(const std::filesystem::path& path);
    bool FileExists(const std::filesystem::path& path);
    bool mkdir_impl(const std::filesystem::path& path);
    bool mkdir_p(const std::filesystem::path& path);

    bool DirectoryIsEmpty(const std::filesystem::path& path);

    void ChDir(const std::filesystem::path&);

    json JSONParseFile(const std::filesystem::path& file);

    bool TryReadFile(const std::filesystem::path& path, std::vector<uint8_t>& out);
    bool TryReadFile(const std::filesystem::path& path, std::string& out);
    bool TryWriteFile(const std::filesystem::path& path, const std::vector<uint8_t>& data);
    bool TryWriteFile(const std::filesystem::path& path, const std::string& data);

    std::vector<std::filesystem::path> ListContents(const std::filesystem::path& path, bool recursive = false, const std::vector<std::string>& ignorelist = {});
    int GetNumberOfSubdirectories(const std::filesystem::path& path, const std::vector<std::string>& ignorelist = {});
    int GetNumberOfFiles(const std::filesystem::path& path, const std::string& extension = "", const std::vector<std::string>& ignorelist = {});

#ifdef __WINDOWS__
    std::string UnicodeToUTF8(const std::wstring& str);
    std::wstring UTF8ToUnicode(const std::string& str);
#endif
}