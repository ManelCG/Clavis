#pragma once

#include <error/ClavisError.h>
#include <filesystem>

namespace Clavis::PasswordStoreElements {
    enum class PasswordStoreElementType {
        UNDEFINED = 0,

        FOLDER = 1,
        GPG_FILE = 2,
        FILE_WITHOUT_EXTENSION = 3,

        UNKNOWN = ~0,
    };

    std::string PasswordStoreElementTypeToString(const PasswordStoreElementType& type);



    class PasswordStoreElement {
    public:
        PasswordStoreElement(std::filesystem::path _p);

        PasswordStoreElementType GetType() const;
        std::filesystem::path GetPath() const;
        std::string GetName() const;

        bool IsGPGFile() const;
        bool IsFolder() const;
        bool IsHiddenFile() const;

        std::string ToString() const;


    protected:

    private:
        std::filesystem::path path;
        PasswordStoreElementType type;
    };
}