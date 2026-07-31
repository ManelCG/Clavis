#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Clavis::Crypto::Base64 {
    // Accepts both the standard (+/) and URL-safe (-_) alphabets in the same pass, and treats
    // padding as optional. Google Authenticator's migration export uses the standard alphabet,
    // while several third-party exporters use the URL-safe one.
    bool TryDecode(const std::string& input, std::vector<uint8_t>& out);

    std::string Encode(const std::vector<uint8_t>& data, bool urlSafe = false, bool pad = true);
}
