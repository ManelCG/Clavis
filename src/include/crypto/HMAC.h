#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <crypto/Hash.h>

namespace Clavis::Crypto {
    // RFC 2104. Intermediate key material is securely zeroed before returning.
    std::vector<uint8_t> HMAC(HashAlgorithm algorithm,
                              const std::vector<uint8_t>& key,
                              const std::vector<uint8_t>& message);

    std::vector<uint8_t> HMAC(HashAlgorithm algorithm,
                              const std::string& key,
                              const std::string& message);
}
