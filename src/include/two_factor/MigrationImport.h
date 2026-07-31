#pragma once

#include <string>
#include <vector>

#include <two_factor/TwoFactorEntry.h>

namespace Clavis::TwoFactor {
    struct MigrationResult {
        std::vector<TwoFactorEntry> entries;

        // Accounts that decoded correctly but that Clavis cannot generate codes for (currently
        // only MD5). Surfaced rather than dropped, so the user knows what was left behind.
        std::vector<std::string> unsupportedAccounts;
    };

    // Decodes Google Authenticator's bulk export:
    //     otpauth-migration://offline?data=<base64 protobuf>
    bool TryParseMigrationUri(const std::string& uri, MigrationResult& out);
}
