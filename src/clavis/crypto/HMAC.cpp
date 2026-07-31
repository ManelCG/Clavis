#include <crypto/HMAC.h>

#include <algorithm>

#include <system/Extensions.h>

namespace Clavis::Crypto {
    std::vector<uint8_t> HMAC(HashAlgorithm algorithm,
                              const std::vector<uint8_t>& key,
                              const std::vector<uint8_t>& message) {
        const auto blockSize = GetBlockSize(algorithm);

        // RFC 2104: keys longer than the block are hashed down first, shorter ones zero-padded.
        std::vector<uint8_t> paddedKey(blockSize, 0);
        if (key.size() > blockSize) {
            const auto hashedKey = Hash(algorithm, key);
            std::copy(hashedKey.begin(), hashedKey.end(), paddedKey.begin());
        } else {
            std::copy(key.begin(), key.end(), paddedKey.begin());
        }

        std::vector<uint8_t> innerInput(blockSize);
        std::vector<uint8_t> outerInput(blockSize);
        for (size_t i = 0; i < blockSize; i++) {
            innerInput[i] = static_cast<uint8_t>(paddedKey[i] ^ 0x36);
            outerInput[i] = static_cast<uint8_t>(paddedKey[i] ^ 0x5c);
        }

        innerInput.insert(innerInput.end(), message.begin(), message.end());
        const auto innerDigest = Hash(algorithm, innerInput);

        outerInput.insert(outerInput.end(), innerDigest.begin(), innerDigest.end());
        auto result = Hash(algorithm, outerInput);

        System::SecureZero(paddedKey.data(), paddedKey.size());
        System::SecureZero(innerInput.data(), innerInput.size());
        System::SecureZero(outerInput.data(), outerInput.size());

        return result;
    }

    std::vector<uint8_t> HMAC(HashAlgorithm algorithm,
                              const std::string& key,
                              const std::string& message) {
        const std::vector<uint8_t> keyBytes(key.begin(), key.end());
        const std::vector<uint8_t> messageBytes(message.begin(), message.end());

        return HMAC(algorithm, keyBytes, messageBytes);
    }
}
