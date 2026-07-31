#pragma once

#include <string>

namespace Clavis::TwoFactor {
    // 2FA entries are GPG-encrypted exactly like passwords, but deliberately do NOT carry the
    // .gpg extension. That keeps them invisible to `pass` (which only globs *.gpg) and lets
    // Clavis tell a token from a password without decrypting anything.
    constexpr const char* TWOFA_EXTENSION = ".2fa";

    constexpr const char* OTPAUTH_SCHEME = "otpauth://";
    constexpr const char* OTPAUTH_MIGRATION_SCHEME = "otpauth-migration://";

    constexpr const char* METADATA_KEY_RECOVERY = "recovery";
    constexpr const char* METADATA_KEY_NOTES = "notes";

    enum class OtpType {
        TOTP,
        HOTP,
    };

    // Steam uses a stock TOTP HMAC but truncates into its own 5-character alphabet instead of
    // decimal digits, so it is an encoder choice rather than a separate OTP type.
    enum class OtpEncoder {
        DEFAULT,
        STEAM,
    };

    constexpr int DEFAULT_DIGITS = 6;
    constexpr int DEFAULT_PERIOD = 30;

    constexpr int MIN_DIGITS = 4;
    constexpr int MAX_DIGITS = 10;

    constexpr int STEAM_DIGITS = 5;
    constexpr int STEAM_PERIOD = 30;
    constexpr const char* STEAM_ALPHABET = "23456789BCDFGHJKMNPQRTVWXY";
}
