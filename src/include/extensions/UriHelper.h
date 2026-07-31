#pragma once

#include <string>
#include <utility>
#include <vector>

namespace Clavis::UriHelper {
    // RFC 3986. Characters in the unreserved set (A-Z a-z 0-9 - . _ ~) are passed through;
    // everything else becomes %XX.
    std::string PercentEncode(const std::string& s, const std::string& extraSafeCharacters = "");

    // '+' is NOT decoded as a space: otpauth:// is a plain URI, not a form-encoded body, and a
    // literal '+' does occur inside account names.
    bool TryPercentDecode(const std::string& s, std::string& out);

    // Splits "a=1&b=2" into pairs. Keys and values are percent-decoded; keys are lowercased,
    // since the otpauth parameter names are case-insensitive in practice.
    std::vector<std::pair<std::string, std::string>> ParseQuery(const std::string& query);

    bool TryGetQueryValue(const std::vector<std::pair<std::string, std::string>>& query,
                          const std::string& key,
                          std::string& out);
}
