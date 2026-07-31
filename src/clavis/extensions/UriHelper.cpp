#include <extensions/UriHelper.h>

#include <extensions/StringHelper.h>

namespace Clavis::UriHelper {
    namespace {
        bool IsUnreserved(char c) {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                   c == '-' || c == '.' || c == '_' || c == '~';
        }

        int HexValue(char c) {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;

            return -1;
        }
    }

    std::string PercentEncode(const std::string& s, const std::string& extraSafeCharacters) {
        constexpr const char* hexDigits = "0123456789ABCDEF";

        std::string out;
        for (const char c : s) {
            if (IsUnreserved(c) || extraSafeCharacters.find(c) != std::string::npos) {
                out += c;
                continue;
            }

            const auto byte = static_cast<unsigned char>(c);
            out += '%';
            out += hexDigits[(byte >> 4) & 0x0F];
            out += hexDigits[byte & 0x0F];
        }

        return out;
    }

    bool TryPercentDecode(const std::string& s, std::string& out) {
        out.clear();

        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] != '%') {
                out += s[i];
                continue;
            }

            if (i + 2 >= s.size())
                return false;

            const int high = HexValue(s[i + 1]);
            const int low = HexValue(s[i + 2]);
            if (high < 0 || low < 0)
                return false;

            out += static_cast<char>((high << 4) | low);
            i += 2;
        }

        return true;
    }

    std::vector<std::pair<std::string, std::string>> ParseQuery(const std::string& query) {
        std::vector<std::pair<std::string, std::string>> result;

        for (const auto& part : StringHelper::Split(query, '&', false)) {
            const auto separator = part.find('=');

            std::string rawKey = separator == std::string::npos ? part : part.substr(0, separator);
            std::string rawValue = separator == std::string::npos ? "" : part.substr(separator + 1);

            std::string key, value;
            if (!TryPercentDecode(rawKey, key) || !TryPercentDecode(rawValue, value))
                continue;

            result.emplace_back(StringHelper::ToLower(key), value);
        }

        return result;
    }

    bool TryGetQueryValue(const std::vector<std::pair<std::string, std::string>>& query,
                          const std::string& key,
                          std::string& out) {
        for (const auto& [k, v] : query) {
            if (k != key)
                continue;

            out = v;
            return true;
        }

        return false;
    }
}
