#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Clavis::Crypto::Base32 {
    // RFC 4648 alphabet (A-Z, 2-7). Decoding is deliberately lenient because real-world issuers
    // present secrets in every imaginable shape: lowercase, grouped in fours with spaces, or
    // hyphenated. Padding is optional -- most sites omit it entirely.
    bool TryDecode(const std::string& input, std::vector<uint8_t>& out);

    std::string Encode(const std::vector<uint8_t>& data, bool pad = false);
}
