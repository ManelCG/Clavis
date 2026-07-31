#include <crypto/Base32.h>

namespace Clavis::Crypto::Base32 {
    namespace {
        constexpr const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

        // Returns the 5-bit value of a Base32 character, or -1 if it is not part of the alphabet.
        int DecodeCharacter(char c) {
            if (c >= 'A' && c <= 'Z')
                return c - 'A';
            if (c >= 'a' && c <= 'z')
                return c - 'a';
            if (c >= '2' && c <= '7')
                return c - '2' + 26;

            return -1;
        }

        bool IsIgnorable(char c) {
            return c == ' ' || c == '-' || c == '\t' || c == '\r' || c == '\n' || c == '=';
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

            buffer = (buffer << 5) | static_cast<uint32_t>(value);
            bitsInBuffer += 5;

            if (bitsInBuffer >= 8) {
                bitsInBuffer -= 8;
                out.push_back(static_cast<uint8_t>((buffer >> bitsInBuffer) & 0xFF));
            }
        }

        // Any leftover bits must be zero padding. Non-zero leftovers mean the input was truncated
        // mid-character, which we reject rather than silently accept a wrong secret.
        if (bitsInBuffer > 0 && (buffer & ((1u << bitsInBuffer) - 1)) != 0)
            return false;

        return true;
    }

    std::string Encode(const std::vector<uint8_t>& data, bool pad) {
        std::string out;

        uint32_t buffer = 0;
        int bitsInBuffer = 0;

        for (const uint8_t byte : data) {
            buffer = (buffer << 8) | byte;
            bitsInBuffer += 8;

            while (bitsInBuffer >= 5) {
                bitsInBuffer -= 5;
                out += ALPHABET[(buffer >> bitsInBuffer) & 0x1F];
            }
        }

        if (bitsInBuffer > 0)
            out += ALPHABET[(buffer << (5 - bitsInBuffer)) & 0x1F];

        if (pad) {
            while (out.size() % 8 != 0)
                out += '=';
        }

        return out;
    }
}
