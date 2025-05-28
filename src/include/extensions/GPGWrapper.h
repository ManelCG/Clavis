#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace Clavis {
    class GPG {
    public:
        static bool TryDecrypt(const std::filesystem::path& path, std::string& out);
        static bool TryDecrypt(const std::vector<uint8_t>& data, std::string& out);

        static bool TryEncrypt(const std::string& data, std::vector<uint8_t>& out);

    protected:

    private:
        static std::string GetGPGID();
    };
}
