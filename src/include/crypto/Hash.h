#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace Clavis::Crypto {
    enum class HashAlgorithm {
        SHA1,
        SHA256,
        SHA512,
    };

    // Size of the digest produced by the algorithm, in bytes.
    size_t GetDigestSize(HashAlgorithm algorithm);

    // Size of the internal block of the algorithm, in bytes. HMAC needs this.
    size_t GetBlockSize(HashAlgorithm algorithm);

    std::string HashAlgorithmToString(HashAlgorithm algorithm);
    bool TryParseHashAlgorithm(const std::string& name, HashAlgorithm& outAlgorithm);

    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const uint8_t* data, size_t size);
    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const std::vector<uint8_t>& data);
    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const std::string& data);
}
