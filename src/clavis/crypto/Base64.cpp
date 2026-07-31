#include <crypto/Base64.h>

namespace Clavis::Crypto::Base64 {
    namespace {
        constexpr const char* STANDARD_ALPHABET =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        constexpr const char* URLSAFE_ALPHABET =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

        int DecodeCharacter(char c) {
            if (c >= 'A' && c <= 'Z')
                return c - 'A';
            if (c >= 'a' && c <= 'z')
                return c - 'a' + 26;
            if (c >= '0' && c <= '9')
                return c - '0' + 52;
            if (c == '+' || c == '-')
                return 62;
            if (c == '/' || c == '_')
                return 63;

            return -1;
        }

        bool IsIgnorable(char c) {
            return c == '=' || c == '\r' || c == '\n' || c == ' ' || c == '\t';
        }
    }

    bool TryDecode(const std::string& input, std::vector<uint8_t>& out) {
        out.clear();

        uint32_t buffer = 0;
        int bitsInBuffer = 0;

        for (const char c : input) {
            if (IsIgnorable(c))
                continue;

            const int value = DecodeCharacter(c);
            if (value < 0)
                return false;

            buffer = (buffer << 6) | static_cast<uint32_t>(value);
            bitsInBuffer += 6;

            if (bitsInBuffer >= 8) {
                bitsInBuffer -= 8;
                out.push_back(static_cast<uint8_t>((buffer >> bitsInBuffer) & 0xFF));
            }
        }

        // A trailing group of 6 leftover bits cannot encode anything -- that is a truncated input.
        if (bitsInBuffer >= 6)
            return false;

        return true;
    }

    std::string Encode(const std::vector<uint8_t>& data, bool urlSafe, bool pad) {
        const char* alphabet = urlSafe ? URLSAFE_ALPHABET : STANDARD_ALPHABET;

        std::string out;

        uint32_t buffer = 0;
        int bitsInBuffer = 0;

        for (const uint8_t byte : data) {
            buffer = (buffer << 8) | byte;
            bitsInBuffer += 8;

            while (bitsInBuffer >= 6) {
                bitsInBuffer -= 6;
                out += alphabet[(buffer >> bitsInBuffer) & 0x3F];
            }
        }

        if (bitsInBuffer > 0)
            out += alphabet[(buffer << (6 - bitsInBuffer)) & 0x3F];

        if (pad) {
            while (out.size() % 4 != 0)
                out += '=';
        }

        return out;
    }
}
