#pragma once

#include <string>
#include <cstddef>

#include <filesystem>
#include <tools/json.hpp>
using json = nlohmann::json;

#ifdef __WINDOWS__
#include <windows.h>
#else
#ifdef __MACOS__
// Ask <string.h> for the C11 Annex K bounds-checked interfaces (memset_s).
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <string.h>
#endif

namespace Clavis::System {

    inline void SecureZero(void* ptr, std::size_t size) {
#ifdef __WINDOWS__
        SecureZeroMemory(ptr, size);
#elif defined(__MACOS__)
        // macOS has no explicit_bzero(); memset_s() is the C11 Annex K equivalent
        // that the compiler is not allowed to optimise away.
        memset_s(ptr, size, 0, size);
#else
        explicit_bzero(ptr, size);
#endif
    }
    bool CopyToClipboard(const std::string& text);

    std::filesystem::path GetHomeFolder();
    std::filesystem::path GetConfigFolder();
    std::filesystem::path GetClavisConfigFolder();
    std::filesystem::path GetPasswordStoreDefaultFolder();
    std::filesystem::path GetPasswordStoreFolder();
    std::filesystem::path GetAssetsFolder();

#ifdef __WINDOWS__
    std::filesystem::path GetAppDataFolder();
#endif

    std::filesystem::path GetGPGIDPath();
    std::filesystem::path GetWorkspacesFilePath();
    std::filesystem::path GetExecutableLocation();

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