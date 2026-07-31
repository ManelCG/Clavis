#include <crypto/Hash.h>

#include <algorithm>
#include <cstring>

#include <error/ClavisError.h>
#include <extensions/StringHelper.h>

// SHA-1 (RFC 3174) and SHA-2 (FIPS 180-4).
//
// These are implemented here rather than pulled from libgcrypt because gpgme exposes no hashing
// primitives, and linking a whole crypto library for ~400 lines of well-specified code would mean
// touching every packaging path (CMakeLists, all four make.sh dependency installers, both
// PKGBUILDs and the Windows installer). Nothing here protects data at rest -- GPG does that. These
// digests exist only to feed HMAC for OTP code generation.
//
// Correctness is pinned by the official test vectors in Crypto::RunSelfTest (crypto/SelfTest.cpp).

namespace Clavis::Crypto {
    namespace {
#pragma region HELPERS
        inline uint32_t RotateLeft32(uint32_t value, int bits) {
            return (value << bits) | (value >> (32 - bits));
        }

        inline uint32_t RotateRight32(uint32_t value, int bits) {
            return (value >> bits) | (value << (32 - bits));
        }

        inline uint64_t RotateRight64(uint64_t value, int bits) {
            return (value >> bits) | (value << (64 - bits));
        }

        inline uint32_t LoadBigEndian32(const uint8_t* p) {
            return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
                   (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
        }

        inline uint64_t LoadBigEndian64(const uint8_t* p) {
            uint64_t result = 0;
            for (int i = 0; i < 8; i++)
                result = (result << 8) | static_cast<uint64_t>(p[i]);
            return result;
        }

        inline void StoreBigEndian32(uint8_t* p, uint32_t value) {
            p[0] = static_cast<uint8_t>(value >> 24);
            p[1] = static_cast<uint8_t>(value >> 16);
            p[2] = static_cast<uint8_t>(value >> 8);
            p[3] = static_cast<uint8_t>(value);
        }

        inline void StoreBigEndian64(uint8_t* p, uint64_t value) {
            for (int i = 0; i < 8; i++)
                p[i] = static_cast<uint8_t>(value >> (56 - 8 * i));
        }

        // Builds the padded message: data || 0x80 || 0x00... || bitLength.
        // lengthFieldSize is 8 for SHA-1/SHA-256 and 16 for SHA-512.
        std::vector<uint8_t> PadMessage(const uint8_t* data, size_t size, size_t blockSize, size_t lengthFieldSize) {
            const uint64_t bitLength = static_cast<uint64_t>(size) * 8;

            size_t paddedSize = size + 1;
            while ((paddedSize + lengthFieldSize) % blockSize != 0)
                paddedSize++;

            std::vector<uint8_t> padded(paddedSize + lengthFieldSize, 0);
            if (size > 0)
                std::memcpy(padded.data(), data, size);
            padded[size] = 0x80;

            // The high half of a SHA-512 length field is always zero for any input we can hold in
            // memory, so writing only the low 64 bits is correct for both variants.
            StoreBigEndian64(padded.data() + padded.size() - 8, bitLength);

            return padded;
        }
#pragma endregion

#pragma region SHA-1
        std::vector<uint8_t> SHA1(const uint8_t* data, size_t size) {
            uint32_t h[5] = { 0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u };

            const auto padded = PadMessage(data, size, 64, 8);

            for (size_t offset = 0; offset < padded.size(); offset += 64) {
                const uint8_t* block = padded.data() + offset;

                uint32_t w[80];
                for (int i = 0; i < 16; i++)
                    w[i] = LoadBigEndian32(block + i * 4);
                for (int i = 16; i < 80; i++)
                    w[i] = RotateLeft32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

                uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];

                for (int i = 0; i < 80; i++) {
                    uint32_t f, k;

                    if (i < 20) {
                        f = (b & c) | ((~b) & d);
                        k = 0x5A827999u;
                    } else if (i < 40) {
                        f = b ^ c ^ d;
                        k = 0x6ED9EBA1u;
                    } else if (i < 60) {
                        f = (b & c) | (b & d) | (c & d);
                        k = 0x8F1BBCDCu;
                    } else {
                        f = b ^ c ^ d;
                        k = 0xCA62C1D6u;
                    }

                    const uint32_t temp = RotateLeft32(a, 5) + f + e + k + w[i];
                    e = d;
                    d = c;
                    c = RotateLeft32(b, 30);
                    b = a;
                    a = temp;
                }

                h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
            }

            std::vector<uint8_t> digest(20);
            for (int i = 0; i < 5; i++)
                StoreBigEndian32(digest.data() + i * 4, h[i]);

            return digest;
        }
#pragma endregion

#pragma region SHA-256
        constexpr uint32_t SHA256_K[64] = {
            0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
            0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
            0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
            0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
            0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
            0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
            0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
            0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
        };

        std::vector<uint8_t> SHA256(const uint8_t* data, size_t size) {
            uint32_t h[8] = {
                0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
                0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u,
            };

            const auto padded = PadMessage(data, size, 64, 8);

            for (size_t offset = 0; offset < padded.size(); offset += 64) {
                const uint8_t* block = padded.data() + offset;

                uint32_t w[64];
                for (int i = 0; i < 16; i++)
                    w[i] = LoadBigEndian32(block + i * 4);
                for (int i = 16; i < 64; i++) {
                    const uint32_t s0 = RotateRight32(w[i - 15], 7) ^ RotateRight32(w[i - 15], 18) ^ (w[i - 15] >> 3);
                    const uint32_t s1 = RotateRight32(w[i - 2], 17) ^ RotateRight32(w[i - 2], 19) ^ (w[i - 2] >> 10);
                    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                }

                uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
                uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

                for (int i = 0; i < 64; i++) {
                    const uint32_t S1 = RotateRight32(e, 6) ^ RotateRight32(e, 11) ^ RotateRight32(e, 25);
                    const uint32_t ch = (e & f) ^ ((~e) & g);
                    const uint32_t temp1 = hh + S1 + ch + SHA256_K[i] + w[i];
                    const uint32_t S0 = RotateRight32(a, 2) ^ RotateRight32(a, 13) ^ RotateRight32(a, 22);
                    const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                    const uint32_t temp2 = S0 + maj;

                    hh = g;
                    g = f;
                    f = e;
                    e = d + temp1;
                    d = c;
                    c = b;
                    b = a;
                    a = temp1 + temp2;
                }

                h[0] += a; h[1] += b; h[2] += c; h[3] += d;
                h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
            }

            std::vector<uint8_t> digest(32);
            for (int i = 0; i < 8; i++)
                StoreBigEndian32(digest.data() + i * 4, h[i]);

            return digest;
        }
#pragma endregion

#pragma region SHA-512
        constexpr uint64_t SHA512_K[80] = {
            0x428a2f98d728ae22ull, 0x7137449123ef65cdull, 0xb5c0fbcfec4d3b2full, 0xe9b5dba58189dbbcull,
            0x3956c25bf348b538ull, 0x59f111f1b605d019ull, 0x923f82a4af194f9bull, 0xab1c5ed5da6d8118ull,
            0xd807aa98a3030242ull, 0x12835b0145706fbeull, 0x243185be4ee4b28cull, 0x550c7dc3d5ffb4e2ull,
            0x72be5d74f27b896full, 0x80deb1fe3b1696b1ull, 0x9bdc06a725c71235ull, 0xc19bf174cf692694ull,
            0xe49b69c19ef14ad2ull, 0xefbe4786384f25e3ull, 0x0fc19dc68b8cd5b5ull, 0x240ca1cc77ac9c65ull,
            0x2de92c6f592b0275ull, 0x4a7484aa6ea6e483ull, 0x5cb0a9dcbd41fbd4ull, 0x76f988da831153b5ull,
            0x983e5152ee66dfabull, 0xa831c66d2db43210ull, 0xb00327c898fb213full, 0xbf597fc7beef0ee4ull,
            0xc6e00bf33da88fc2ull, 0xd5a79147930aa725ull, 0x06ca6351e003826full, 0x142929670a0e6e70ull,
            0x27b70a8546d22ffcull, 0x2e1b21385c26c926ull, 0x4d2c6dfc5ac42aedull, 0x53380d139d95b3dfull,
            0x650a73548baf63deull, 0x766a0abb3c77b2a8ull, 0x81c2c92e47edaee6ull, 0x92722c851482353bull,
            0xa2bfe8a14cf10364ull, 0xa81a664bbc423001ull, 0xc24b8b70d0f89791ull, 0xc76c51a30654be30ull,
            0xd192e819d6ef5218ull, 0xd69906245565a910ull, 0xf40e35855771202aull, 0x106aa07032bbd1b8ull,
            0x19a4c116b8d2d0c8ull, 0x1e376c085141ab53ull, 0x2748774cdf8eeb99ull, 0x34b0bcb5e19b48a8ull,
            0x391c0cb3c5c95a63ull, 0x4ed8aa4ae3418acbull, 0x5b9cca4f7763e373ull, 0x682e6ff3d6b2b8a3ull,
            0x748f82ee5defb2fcull, 0x78a5636f43172f60ull, 0x84c87814a1f0ab72ull, 0x8cc702081a6439ecull,
            0x90befffa23631e28ull, 0xa4506cebde82bde9ull, 0xbef9a3f7b2c67915ull, 0xc67178f2e372532bull,
            0xca273eceea26619cull, 0xd186b8c721c0c207ull, 0xeada7dd6cde0eb1eull, 0xf57d4f7fee6ed178ull,
            0x06f067aa72176fbaull, 0x0a637dc5a2c898a6ull, 0x113f9804bef90daeull, 0x1b710b35131c471bull,
            0x28db77f523047d84ull, 0x32caab7b40c72493ull, 0x3c9ebe0a15c9bebcull, 0x431d67c49c100d4cull,
            0x4cc5d4becb3e42b6ull, 0x597f299cfc657e2aull, 0x5fcb6fab3ad6faecull, 0x6c44198c4a475817ull,
        };

        std::vector<uint8_t> SHA512(const uint8_t* data, size_t size) {
            uint64_t h[8] = {
                0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull, 0x3c6ef372fe94f82bull, 0xa54ff53a5f1d36f1ull,
                0x510e527fade682d1ull, 0x9b05688c2b3e6c1full, 0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull,
            };

            const auto padded = PadMessage(data, size, 128, 16);

            for (size_t offset = 0; offset < padded.size(); offset += 128) {
                const uint8_t* block = padded.data() + offset;

                uint64_t w[80];
                for (int i = 0; i < 16; i++)
                    w[i] = LoadBigEndian64(block + i * 8);
                for (int i = 16; i < 80; i++) {
                    const uint64_t s0 = RotateRight64(w[i - 15], 1) ^ RotateRight64(w[i - 15], 8) ^ (w[i - 15] >> 7);
                    const uint64_t s1 = RotateRight64(w[i - 2], 19) ^ RotateRight64(w[i - 2], 61) ^ (w[i - 2] >> 6);
                    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
                }

                uint64_t a = h[0], b = h[1], c = h[2], d = h[3];
                uint64_t e = h[4], f = h[5], g = h[6], hh = h[7];

                for (int i = 0; i < 80; i++) {
                    const uint64_t S1 = RotateRight64(e, 14) ^ RotateRight64(e, 18) ^ RotateRight64(e, 41);
                    const uint64_t ch = (e & f) ^ ((~e) & g);
                    const uint64_t temp1 = hh + S1 + ch + SHA512_K[i] + w[i];
                    const uint64_t S0 = RotateRight64(a, 28) ^ RotateRight64(a, 34) ^ RotateRight64(a, 39);
                    const uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
                    const uint64_t temp2 = S0 + maj;

                    hh = g;
                    g = f;
                    f = e;
                    e = d + temp1;
                    d = c;
                    c = b;
                    b = a;
                    a = temp1 + temp2;
                }

                h[0] += a; h[1] += b; h[2] += c; h[3] += d;
                h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
            }

            std::vector<uint8_t> digest(64);
            for (int i = 0; i < 8; i++)
                StoreBigEndian64(digest.data() + i * 8, h[i]);

            return digest;
        }
#pragma endregion
    }

    size_t GetDigestSize(HashAlgorithm algorithm) {
        switch (algorithm) {
            case HashAlgorithm::SHA1:   return 20;
            case HashAlgorithm::SHA256: return 32;
            case HashAlgorithm::SHA512: return 64;
        }

        RaiseClavisError("Unknown hash algorithm");
    }

    size_t GetBlockSize(HashAlgorithm algorithm) {
        switch (algorithm) {
            case HashAlgorithm::SHA1:   return 64;
            case HashAlgorithm::SHA256: return 64;
            case HashAlgorithm::SHA512: return 128;
        }

        RaiseClavisError("Unknown hash algorithm");
    }

    std::string HashAlgorithmToString(HashAlgorithm algorithm) {
        switch (algorithm) {
            case HashAlgorithm::SHA1:   return "SHA1";
            case HashAlgorithm::SHA256: return "SHA256";
            case HashAlgorithm::SHA512: return "SHA512";
        }

        RaiseClavisError("Unknown hash algorithm");
    }

    bool TryParseHashAlgorithm(const std::string& name, HashAlgorithm& outAlgorithm) {
        auto normalized = StringHelper::ToLower(name);

        // Some issuers write "SHA-256" rather than "SHA256".
        normalized.erase(std::remove(normalized.begin(), normalized.end(), '-'), normalized.end());

        if (normalized == "sha1") {
            outAlgorithm = HashAlgorithm::SHA1;
            return true;
        }
        if (normalized == "sha256") {
            outAlgorithm = HashAlgorithm::SHA256;
            return true;
        }
        if (normalized == "sha512") {
            outAlgorithm = HashAlgorithm::SHA512;
            return true;
        }

        return false;
    }

    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const uint8_t* data, size_t size) {
        switch (algorithm) {
            case HashAlgorithm::SHA1:   return SHA1(data, size);
            case HashAlgorithm::SHA256: return SHA256(data, size);
            case HashAlgorithm::SHA512: return SHA512(data, size);
        }

        RaiseClavisError("Unknown hash algorithm");
    }

    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const std::vector<uint8_t>& data) {
        return Hash(algorithm, data.data(), data.size());
    }

    std::vector<uint8_t> Hash(HashAlgorithm algorithm, const std::string& data) {
        return Hash(algorithm, reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }
}
