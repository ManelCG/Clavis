#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <crypto/Hash.h>
#include <two_factor/TwoFactorDefinitions.h>

namespace Clavis::TwoFactor::OTP {
    // RFC 4226 section 5.3. Counter is serialized big-endian over 8 bytes.
    std::string GenerateHOTP(Crypto::HashAlgorithm algorithm,
                             const std::vector<uint8_t>& secret,
                             uint64_t counter,
                             int digits);

    // RFC 6238. counter = floor((unixTime - t0) / period)
    std::string GenerateTOTP(Crypto::HashAlgorithm algorithm,
                             const std::vector<uint8_t>& secret,
                             int64_t unixTime,
                             int period,
                             int digits,
                             int64_t t0 = 0);

    // Steam Guard: HMAC-SHA1 over the same time counter, truncated into STEAM_ALPHABET.
    std::string GenerateSteam(const std::vector<uint8_t>& secret,
                              int64_t unixTime,
                              int period = STEAM_PERIOD,
                              int64_t t0 = 0);

    int64_t GetUnixTime();

    uint64_t GetTimeCounter(int64_t unixTime, int period, int64_t t0 = 0);

    // Seconds until the current time-step expires. Always in [1, period].
    int GetSecondsRemaining(int64_t unixTime, int period, int64_t t0 = 0);
}
