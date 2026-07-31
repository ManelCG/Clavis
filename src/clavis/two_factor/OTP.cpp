#include <two_factor/OTP.h>

#include <algorithm>
#include <ctime>

#include <crypto/HMAC.h>
#include <system/Extensions.h>

namespace Clavis::TwoFactor::OTP {
    namespace {
        // RFC 4226 section 5.3. The offset is taken from the LAST byte of the digest, not from
        // byte 19 -- that distinction is what makes the SHA-256 and SHA-512 variants produce the
        // codes published in RFC 6238 Appendix B.
        uint32_t DynamicTruncate(const std::vector<uint8_t>& digest) {
            const size_t offset = static_cast<size_t>(digest.back() & 0x0F);

            return ((static_cast<uint32_t>(digest[offset]) & 0x7F) << 24) |
                   ((static_cast<uint32_t>(digest[offset + 1]) & 0xFF) << 16) |
                   ((static_cast<uint32_t>(digest[offset + 2]) & 0xFF) << 8) |
                   (static_cast<uint32_t>(digest[offset + 3]) & 0xFF);
        }

        std::vector<uint8_t> CounterToBytes(uint64_t counter) {
            std::vector<uint8_t> bytes(8);
            for (int i = 0; i < 8; i++)
                bytes[i] = static_cast<uint8_t>(counter >> (56 - 8 * i));

            return bytes;
        }
    }

    std::string GenerateHOTP(Crypto::HashAlgorithm algorithm,
                             const std::vector<uint8_t>& secret,
                             uint64_t counter,
                             int digits) {
        digits = std::clamp(digits, MIN_DIGITS, MAX_DIGITS);

        auto message = CounterToBytes(counter);
        auto digest = Crypto::HMAC(algorithm, secret, message);

        const uint32_t truncated = DynamicTruncate(digest);

        uint64_t modulo = 1;
        for (int i = 0; i < digits; i++)
            modulo *= 10;

        auto code = std::to_string(static_cast<uint64_t>(truncated) % modulo);
        while (static_cast<int>(code.size()) < digits)
            code.insert(code.begin(), '0');

        System::SecureZero(digest.data(), digest.size());

        return code;
    }

    std::string GenerateTOTP(Crypto::HashAlgorithm algorithm,
                             const std::vector<uint8_t>& secret,
                             int64_t unixTime,
                             int period,
                             int digits,
                             int64_t t0) {
        return GenerateHOTP(algorithm, secret, GetTimeCounter(unixTime, period, t0), digits);
    }

    std::string GenerateSteam(const std::vector<uint8_t>& secret,
                              int64_t unixTime,
                              int period,
                              int64_t t0) {
        auto message = CounterToBytes(GetTimeCounter(unixTime, period, t0));
        auto digest = Crypto::HMAC(Crypto::HashAlgorithm::SHA1, secret, message);

        uint32_t value = DynamicTruncate(digest);

        const size_t alphabetSize = std::string(STEAM_ALPHABET).size();

        std::string code;
        for (int i = 0; i < STEAM_DIGITS; i++) {
            code += STEAM_ALPHABET[value % alphabetSize];
            value /= static_cast<uint32_t>(alphabetSize);
        }

        System::SecureZero(digest.data(), digest.size());

        return code;
    }

    int64_t GetUnixTime() {
        return static_cast<int64_t>(std::time(nullptr));
    }

    uint64_t GetTimeCounter(int64_t unixTime, int period, int64_t t0) {
        if (period < 1)
            period = DEFAULT_PERIOD;

        const int64_t elapsed = unixTime - t0;
        if (elapsed < 0)
            return 0;

        return static_cast<uint64_t>(elapsed / period);
    }

    int GetSecondsRemaining(int64_t unixTime, int period, int64_t t0) {
        if (period < 1)
            period = DEFAULT_PERIOD;

        const int64_t elapsed = unixTime - t0;
        if (elapsed < 0)
            return period;

        return period - static_cast<int>(elapsed % period);
    }
}
