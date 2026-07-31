#pragma once

namespace Clavis::Crypto {
    // Runs the published RFC test vectors for SHA-1/256/512, HMAC, Base32, Base64, HOTP and TOTP.
    // Results are printed to stdout. Returns true only if every vector passed.
    //
    // The project has no test framework, so this is the regression suite for the OTP stack:
    //     ./clavis --selftest-crypto
    bool RunSelfTest();
}
