#include <extensions/StringHelper.h>

#include <algorithm>

namespace Clavis::StringHelper {
    std::string ToLower(const std::string &s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](const unsigned char c) { return std::tolower(c); });
        return result;
    }

    std::string TrimTrailingNewlines(const std::string &s) {
        std::string result = s;
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
            result.pop_back();

        return result;
    }

    std::vector<std::string> Split(const std::string &s, char delimiter, bool keepEmpty) {
        std::vector<std::string> result;

        std::string current;
        for (const char c : s) {
            if (c != delimiter) {
                current += c;
                continue;
            }

            if (keepEmpty || !current.empty())
                result.push_back(current);
            current.clear();
        }

        if (keepEmpty || !current.empty())
            result.push_back(current);

        return result;
    }

    std::string Trim(const std::string &s) {
        constexpr const char* whitespace = " \t\r\n";

        const auto first = s.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return "";

        const auto last = s.find_last_not_of(whitespace);

        return s.substr(first, last - first + 1);
    }

    bool StartsWith(const std::string &s, const std::string &prefix) {
        if (prefix.size() > s.size())
            return false;

        return s.compare(0, prefix.size(), prefix) == 0;
    }

    bool EndsWith(const std::string &s, const std::string &suffix) {
        if (suffix.size() > s.size())
            return false;

        return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
}