#pragma once

#include <filesystem>
#include <string>


namespace Clavis {
    class Password {
    public:
        Password();

        static Password FromFile(std::filesystem::path path);
        static Password FromPassword(const std::string& password);

        bool IsOnDisk() const;
        bool IsDecrypted() const;

        bool TryDecrypt();
        bool TrySaveEncrypted(const std::filesystem::path& dest);

        const std::string& GetPassword() const;
        std::filesystem::path GetPath() const;

        ~Password();

    protected:

    private:
        bool isOnDisk;
        bool isDecrypted;

        std::filesystem::path path;
        std::string password;
    };
}