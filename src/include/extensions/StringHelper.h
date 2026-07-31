#pragma once
#include <string>
#include <vector>

namespace Clavis::StringHelper {
    std::string ToLower(const std::string& s);
    std::string TrimTrailingNewlines(const std::string& s);

    std::vector<std::string> Split(const std::string& s, char delimiter, bool keepEmpty = true);
    std::string Trim(const std::string& s);
    bool StartsWith(const std::string& s, const std::string& prefix);
    bool EndsWith(const std::string& s, const std::string& suffix);
}
