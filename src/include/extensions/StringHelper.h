#pragma once
#include <string>

namespace Clavis::StringHelper {
    std::string ToLower(const std::string& s);
    std::string TrimTrailingNewlines(const std::string& s);
}
