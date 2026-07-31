#include <crypto/SelfTest.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <crypto/Base32.h>
#include <crypto/Base64.h>
#include <crypto/HMAC.h>
#include <crypto/Hash.h>
#include <two_factor/OTP.h>
#include <two_factor/MigrationImport.h>
#include <two_factor/TwoFactorEntry.h>
#include <extensions/UriHelper.h>

namespace Clavis::Crypto {
    namespace {
        int totalTests = 0;
        int failedTests = 0;

        std::string ToHex(const std::vector<uint8_t>& data) {
            std::ostringstream stream;
            for (const uint8_t byte : data)
                stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);

            return stream.str();
        }

        std::vector<uint8_t> FromString(const std::string& s) {
            return std::vector<uint8_t>(s.begin(), s.end());
        }

        std::vector<uint8_t> Repeated(uint8_t byte, size_t count) {
            return std::vector<uint8_t>(count, byte);
        }

        void Check(const std::string& name, const std::string& actual, const std::string& expected) {
            totalTests++;

            if (actual == expected) {
                std::cout << "  PASS  " << name << "\n";
                return;
            }

            failedTests++;
            std::cout << "  FAIL  " << name << "\n"
                      << "        expected: " << expected << "\n"
                      << "        actual:   " << actual << "\n";
        }

#pragma region HASH VECTORS
        // FIPS 180-4 / RFC 3174.
        void TestHashes() {
            std::cout << "SHA-1 / SHA-256 / SHA-512 (FIPS 180-4)\n";

            const std::string abc = "abc";
            const std::string empty = "";

            // The 56-character message spans two blocks once the length field is appended, which
            // is the case a single-block implementation silently gets wrong.
            const std::string twoBlocks = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
            const std::string twoBlocks1024 =
                "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno"
                "ijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

            Check("SHA-1(\"\")", ToHex(Hash(HashAlgorithm::SHA1, empty)),
                  "da39a3ee5e6b4b0d3255bfef95601890afd80709");
            Check("SHA-1(\"abc\")", ToHex(Hash(HashAlgorithm::SHA1, abc)),
                  "a9993e364706816aba3e25717850c26c9cd0d89d");
            Check("SHA-1(two blocks)", ToHex(Hash(HashAlgorithm::SHA1, twoBlocks)),
                  "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

            Check("SHA-256(\"\")", ToHex(Hash(HashAlgorithm::SHA256, empty)),
                  "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
            Check("SHA-256(\"abc\")", ToHex(Hash(HashAlgorithm::SHA256, abc)),
                  "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
            Check("SHA-256(two blocks)", ToHex(Hash(HashAlgorithm::SHA256, twoBlocks)),
                  "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");

            Check("SHA-512(\"\")", ToHex(Hash(HashAlgorithm::SHA512, empty)),
                  "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
                  "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
            Check("SHA-512(\"abc\")", ToHex(Hash(HashAlgorithm::SHA512, abc)),
                  "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
                  "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
            Check("SHA-512(two blocks)", ToHex(Hash(HashAlgorithm::SHA512, twoBlocks1024)),
                  "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
                  "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909");
        }
#pragma endregion

#pragma region HMAC VECTORS
        // RFC 2202 (SHA-1) and RFC 4231 (SHA-256/SHA-512).
        void TestHMAC() {
            std::cout << "HMAC (RFC 2202 / RFC 4231)\n";

            const auto key1 = Repeated(0x0b, 20);
            const auto data1 = FromString("Hi There");

            Check("HMAC-SHA1 case 1", ToHex(HMAC(HashAlgorithm::SHA1, key1, data1)),
                  "b617318655057264e28bc0b6fb378c8ef146be00");
            Check("HMAC-SHA256 case 1", ToHex(HMAC(HashAlgorithm::SHA256, key1, data1)),
                  "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
            Check("HMAC-SHA512 case 1", ToHex(HMAC(HashAlgorithm::SHA512, key1, data1)),
                  "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cde"
                  "daa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854");

            const auto key2 = FromString("Jefe");
            const auto data2 = FromString("what do ya want for nothing?");

            Check("HMAC-SHA1 case 2", ToHex(HMAC(HashAlgorithm::SHA1, key2, data2)),
                  "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
            Check("HMAC-SHA256 case 2", ToHex(HMAC(HashAlgorithm::SHA256, key2, data2)),
                  "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
            Check("HMAC-SHA512 case 2", ToHex(HMAC(HashAlgorithm::SHA512, key2, data2)),
                  "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea250554"
                  "9758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737");

            // Keys longer than the block have to be hashed down first. This is the branch that
            // implementations most often get wrong, so it is tested for every algorithm -- note
            // SHA-512 needs the 131-byte key, since its block is 128 bytes.
            const auto longData = FromString("Test Using Larger Than Block-Size Key - Hash Key First");

            Check("HMAC-SHA1 long key", ToHex(HMAC(HashAlgorithm::SHA1, Repeated(0xaa, 80), longData)),
                  "aa4ae5e15272d00e95705637ce8a3b55ed402112");
            Check("HMAC-SHA256 long key", ToHex(HMAC(HashAlgorithm::SHA256, Repeated(0xaa, 131), longData)),
                  "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");
            Check("HMAC-SHA512 long key", ToHex(HMAC(HashAlgorithm::SHA512, Repeated(0xaa, 131), longData)),
                  "80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f352"
                  "6b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598");
        }
#pragma endregion

#pragma region BASE32 / BASE64 VECTORS
        // RFC 4648 section 10.
        void TestBase32() {
            std::cout << "Base32 (RFC 4648)\n";

            const std::vector<std::pair<std::string, std::string>> vectors = {
                { "", "" },
                { "f", "MY======" },
                { "fo", "MZXQ====" },
                { "foo", "MZXW6===" },
                { "foob", "MZXW6YQ=" },
                { "fooba", "MZXW6YTB" },
                { "foobar", "MZXW6YTBOI======" },
            };

            for (const auto& [plain, encoded] : vectors) {
                Check("Base32::Encode(\"" + plain + "\")", Base32::Encode(FromString(plain), true), encoded);

                std::vector<uint8_t> decoded;
                const bool ok = Base32::TryDecode(encoded, decoded);
                Check("Base32::TryDecode(\"" + encoded + "\")",
                      ok ? std::string(decoded.begin(), decoded.end()) : std::string("<failed>"), plain);
            }

            // Real-world secrets arrive lowercase, spaced or hyphenated. All must decode alike.
            std::vector<uint8_t> a, b, c, d;
            Base32::TryDecode("MZXW6YTBOI", a);
            Base32::TryDecode("mzxw6ytboi", b);
            Base32::TryDecode("MZXW 6YTB OI", c);
            Base32::TryDecode("MZXW-6YTB-OI", d);

            Check("Base32 lenient (lowercase)", ToHex(b), ToHex(a));
            Check("Base32 lenient (spaces)", ToHex(c), ToHex(a));
            Check("Base32 lenient (hyphens)", ToHex(d), ToHex(a));

            std::vector<uint8_t> rejected;
            Check("Base32 rejects invalid characters",
                  Base32::TryDecode("MZXW6YTB!!", rejected) ? "accepted" : "rejected", "rejected");
        }

        void TestBase64() {
            std::cout << "Base64 (RFC 4648)\n";

            const std::vector<std::pair<std::string, std::string>> vectors = {
                { "", "" },
                { "f", "Zg==" },
                { "fo", "Zm8=" },
                { "foo", "Zm9v" },
                { "foob", "Zm9vYg==" },
                { "fooba", "Zm9vYmE=" },
                { "foobar", "Zm9vYmFy" },
            };

            for (const auto& [plain, encoded] : vectors) {
                Check("Base64::Encode(\"" + plain + "\")", Base64::Encode(FromString(plain)), encoded);

                std::vector<uint8_t> decoded;
                const bool ok = Base64::TryDecode(encoded, decoded);
                Check("Base64::TryDecode(\"" + encoded + "\")",
                      ok ? std::string(decoded.begin(), decoded.end()) : std::string("<failed>"), plain);
            }

            // The migration payload is binary, so both alphabets must round-trip identically.
            const std::vector<uint8_t> binary = { 0xfb, 0xff, 0xbe, 0x00, 0x10 };
            std::vector<uint8_t> standard, urlSafe;
            Base64::TryDecode(Base64::Encode(binary, false), standard);
            Base64::TryDecode(Base64::Encode(binary, true), urlSafe);

            Check("Base64 standard alphabet round-trip", ToHex(standard), ToHex(binary));
            Check("Base64 URL-safe alphabet round-trip", ToHex(urlSafe), ToHex(binary));
        }
#pragma endregion

#pragma region OTP VECTORS
        // RFC 4226 Appendix D.
        void TestHOTP() {
            std::cout << "HOTP (RFC 4226 Appendix D)\n";

            const auto secret = FromString("12345678901234567890");
            const std::vector<std::string> expected = {
                "755224", "287082", "359152", "969429", "338314",
                "254676", "287922", "162583", "399871", "520489",
            };

            for (size_t counter = 0; counter < expected.size(); counter++)
                Check("HOTP counter " + std::to_string(counter),
                      TwoFactor::OTP::GenerateHOTP(HashAlgorithm::SHA1, secret, counter, 6),
                      expected[counter]);
        }

        // RFC 6238 Appendix B. Each algorithm uses a DIFFERENT seed -- the SHA-256 and SHA-512
        // rows extend the 20-byte SHA-1 seed to 32 and 64 bytes respectively. Reusing the SHA-1
        // seed for all three is the classic way to ship a broken SHA-256 TOTP.
        void TestTOTP() {
            std::cout << "TOTP (RFC 6238 Appendix B)\n";

            const auto seedSha1 = FromString("12345678901234567890");
            const auto seedSha256 = FromString("12345678901234567890123456789012");
            const auto seedSha512 = FromString("1234567890123456789012345678901234567890123456789012345678901234");

            struct Vector {
                int64_t time;
                std::string sha1;
                std::string sha256;
                std::string sha512;
            };

            const std::vector<Vector> vectors = {
                {          59LL, "94287082", "46119246", "90693936" },
                {  1111111109LL, "07081804", "68084774", "25091201" },
                {  1111111111LL, "14050471", "67062674", "99943326" },
                {  1234567890LL, "89005924", "91819424", "93441116" },
                {  2000000000LL, "69279037", "90698825", "38618901" },
                { 20000000000LL, "65353130", "77737706", "47863826" },
            };

            for (const auto& v : vectors) {
                const auto label = std::to_string(v.time);

                Check("TOTP SHA1 T=" + label,
                      TwoFactor::OTP::GenerateTOTP(HashAlgorithm::SHA1, seedSha1, v.time, 30, 8), v.sha1);
                Check("TOTP SHA256 T=" + label,
                      TwoFactor::OTP::GenerateTOTP(HashAlgorithm::SHA256, seedSha256, v.time, 30, 8), v.sha256);
                Check("TOTP SHA512 T=" + label,
                      TwoFactor::OTP::GenerateTOTP(HashAlgorithm::SHA512, seedSha512, v.time, 30, 8), v.sha512);
            }

            // Independent cross-check against oathtool, which shares no code with this
            // implementation. Reproduce with:
            //     oathtool --totp -b --digits=6 --now="2026-01-01 00:00:00 UTC" JBSWY3DPEHPK3PXP
            std::vector<uint8_t> crossCheckSecret;
            Base32::TryDecode("JBSWY3DPEHPK3PXP", crossCheckSecret);
            Check("TOTP cross-check vs oathtool",
                  TwoFactor::OTP::GenerateTOTP(HashAlgorithm::SHA1, crossCheckSecret, 1767225600LL, 30, 6),
                  "260025");

            Check("TOTP seconds remaining at T=0", std::to_string(TwoFactor::OTP::GetSecondsRemaining(0, 30)), "30");
            Check("TOTP seconds remaining at T=1", std::to_string(TwoFactor::OTP::GetSecondsRemaining(1, 30)), "29");
            Check("TOTP seconds remaining at T=29", std::to_string(TwoFactor::OTP::GetSecondsRemaining(29, 30)), "1");
            Check("TOTP seconds remaining at T=30", std::to_string(TwoFactor::OTP::GetSecondsRemaining(30, 30)), "30");
        }
#pragma endregion

#pragma region ENTRY ROUND-TRIPS
        std::string DescribeEntry(const TwoFactor::TwoFactorEntry& e) {
            std::ostringstream s;
            s << (e.GetType() == TwoFactor::OtpType::HOTP ? "hotp" : "totp")
              << "|" << (e.GetEncoder() == TwoFactor::OtpEncoder::STEAM ? "steam" : "default")
              << "|" << HashAlgorithmToString(e.GetAlgorithm())
              << "|issuer=" << e.GetIssuer()
              << "|account=" << e.GetAccount()
              << "|secret=" << e.GetSecretBase32()
              << "|digits=" << e.GetDigits()
              << "|period=" << e.GetPeriod()
              << "|counter=" << e.GetCounter();

            return s.str();
        }

        // Parsing a URI, rebuilding it and parsing it again must be a fixed point. This is what
        // guarantees that "Transfer 2FA" hands another app exactly the credential Clavis holds.
        void CheckUriRoundTrip(const std::string& name, const std::string& uri) {
            TwoFactor::TwoFactorEntry first;
            if (!TwoFactor::TwoFactorEntry::TryFromUri(uri, first)) {
                Check(name, "<parse failed>", "<parsed>");
                return;
            }

            TwoFactor::TwoFactorEntry second;
            if (!TwoFactor::TwoFactorEntry::TryFromUri(first.BuildUri(), second)) {
                Check(name, "<rebuild failed>", "<parsed>");
                return;
            }

            Check(name, DescribeEntry(second), DescribeEntry(first));
        }

        void TestEntryParsing() {
            std::cout << "otpauth:// URI round-trips\n";

            CheckUriRoundTrip("plain TOTP",
                              "otpauth://totp/GitHub:me@example.com?secret=JBSWY3DPEHPK3PXP&issuer=GitHub");
            CheckUriRoundTrip("no issuer",
                              "otpauth://totp/me@example.com?secret=JBSWY3DPEHPK3PXP");
            CheckUriRoundTrip("issuer in label only",
                              "otpauth://totp/GitHub:me?secret=JBSWY3DPEHPK3PXP");
            CheckUriRoundTrip("percent-encoded label",
                              "otpauth://totp/Big%20Corp:me%40example.com?secret=JBSWY3DPEHPK3PXP");
            CheckUriRoundTrip("non-ASCII issuer",
                              "otpauth://totp/Caf%C3%A9:me?secret=JBSWY3DPEHPK3PXP&issuer=Caf%C3%A9");
            CheckUriRoundTrip("SHA256 / 8 digits / period 60",
                              "otpauth://totp/x:y?secret=JBSWY3DPEHPK3PXP&algorithm=SHA256&digits=8&period=60");
            CheckUriRoundTrip("HOTP with counter",
                              "otpauth://hotp/x:y?secret=JBSWY3DPEHPK3PXP&counter=42");
            CheckUriRoundTrip("Steam via encoder",
                              "otpauth://totp/Steam:me?secret=JBSWY3DPEHPK3PXP&encoder=steam");

            // The issuer parameter must win over the label prefix.
            TwoFactor::TwoFactorEntry conflicting;
            TwoFactor::TwoFactorEntry::TryFromUri(
                "otpauth://totp/Wrong:me?secret=JBSWY3DPEHPK3PXP&issuer=Right", conflicting);
            Check("issuer parameter overrides label", conflicting.GetIssuer(), "Right");

            // "Issuer:%20account" is common; the leading space must not survive.
            TwoFactor::TwoFactorEntry spaced;
            TwoFactor::TwoFactorEntry::TryFromUri(
                "otpauth://totp/GitHub:%20me@example.com?secret=JBSWY3DPEHPK3PXP", spaced);
            Check("leading space stripped from account", spaced.GetAccount(), "me@example.com");

            // Steam must be recognised from the issuer alone, and forces its own parameters.
            TwoFactor::TwoFactorEntry steam;
            TwoFactor::TwoFactorEntry::TryFromUri(
                "otpauth://totp/Steam:me?secret=JBSWY3DPEHPK3PXP&issuer=Steam", steam);
            Check("Steam detected from issuer",
                  std::to_string(steam.GetDigits()) + "/" +
                      (steam.GetEncoder() == TwoFactor::OtpEncoder::STEAM ? "steam" : "default"),
                  "5/steam");

            std::string steamCode;
            const bool steamGenerated = steam.TryGenerateCode(1234567890LL, steamCode);
            Check("Steam code generated", steamGenerated ? "ok" : "failed", "ok");
            Check("Steam code is 5 chars from its alphabet",
                  std::to_string(steamCode.size()) + ":" +
                      (steamCode.find_first_not_of(TwoFactor::STEAM_ALPHABET) == std::string::npos
                           ? "ok" : "bad"),
                  "5:ok");

            TwoFactor::TwoFactorEntry rejected;
            Check("MD5 algorithm rejected",
                  TwoFactor::TwoFactorEntry::TryFromUri(
                      "otpauth://totp/x?secret=JBSWY3DPEHPK3PXP&algorithm=MD5", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
            Check("missing secret rejected",
                  TwoFactor::TwoFactorEntry::TryFromUri("otpauth://totp/x?issuer=y", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
            Check("non-otpauth URI rejected",
                  TwoFactor::TwoFactorEntry::TryFromUri("https://example.com", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
        }

        // Serialize -> parse must preserve metadata exactly, including multi-line notes and
        // unknown keys written by a future version or by hand.
        void TestFileFormat() {
            std::cout << ".2fa file format round-trips\n";

            TwoFactor::TwoFactorEntry entry;
            TwoFactor::TwoFactorEntry::TryFromUri(
                "otpauth://totp/GitHub:me@example.com?secret=JBSWY3DPEHPK3PXP&issuer=GitHub", entry);

            entry.SetRecoveryCodes({ "1a2b-3c4d", "5e6f-7g8h", "9i0j-1k2l" });
            entry.SetNotes("line one\nline two");
            auto fields = entry.GetMetadata();
            fields.push_back(TwoFactor::MetadataField{ "future-key", "unknown value" });
            entry.SetMetadata(fields);

            const auto serialized = entry.Serialize();

            TwoFactor::TwoFactorEntry parsed;
            const bool ok = TwoFactor::TwoFactorEntry::TryFromFileContents(serialized, parsed);
            Check("file contents parse", ok ? "ok" : "failed", "ok");

            Check("entry survives round-trip", DescribeEntry(parsed), DescribeEntry(entry));
            Check("recovery codes survive", std::to_string(parsed.GetRecoveryCodes().size()), "3");
            Check("recovery code value survives",
                  parsed.GetRecoveryCodes().empty() ? "" : parsed.GetRecoveryCodes()[1], "5e6f-7g8h");
            Check("multi-line notes survive", parsed.GetNotes(), "line one\nline two");
            Check("serialize is idempotent", parsed.Serialize(), serialized);

            std::string unknown;
            for (const auto& f : parsed.GetMetadata()) {
                if (f.key == "future-key")
                    unknown = f.value;
            }
            Check("unknown metadata keys preserved", unknown, "unknown value");

            // The first line must be a URI; anything else is not a 2FA file.
            TwoFactor::TwoFactorEntry rejected;
            Check("plain password rejected as 2FA file",
                  TwoFactor::TwoFactorEntry::TryFromFileContents("hunter2\nuser: bob", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
        }

        // Builds the protobuf payload byte by byte so the expected encoding is verifiable by
        // inspection, then feeds it through the real Base64 + wire-format path. The secret is
        // "Hello!\xDE\xAD\xBE\xEF", the well-known example whose Base32 form is JBSWY3DPEHPK3PXP.
        void TestMigrationImport() {
            std::cout << "otpauth-migration:// import\n";

            const std::string secret = "Hello!\xDE\xAD\xBE\xEF";
            const std::string name = "Example:alice@google.com";
            const std::string issuer = "Example";

            std::vector<uint8_t> parameters;
            const auto append = [&parameters](uint8_t tag, const std::string& value) {
                parameters.push_back(tag);
                parameters.push_back(static_cast<uint8_t>(value.size()));
                parameters.insert(parameters.end(), value.begin(), value.end());
            };

            append(0x0a, secret);   // field 1, length-delimited: raw secret bytes
            append(0x12, name);     // field 2, length-delimited: name
            append(0x1a, issuer);   // field 3, length-delimited: issuer
            parameters.push_back(0x20); parameters.push_back(0x01);   // field 4 varint: SHA1
            parameters.push_back(0x28); parameters.push_back(0x01);   // field 5 varint: SIX digits
            parameters.push_back(0x30); parameters.push_back(0x02);   // field 6 varint: TOTP

            std::vector<uint8_t> payload;
            payload.push_back(0x0a);                                          // field 1 of the payload
            payload.push_back(static_cast<uint8_t>(parameters.size()));
            payload.insert(payload.end(), parameters.begin(), parameters.end());
            payload.push_back(0x10); payload.push_back(0x01);                 // version = 1
            payload.push_back(0x18); payload.push_back(0x01);                 // batch_size = 1

            const auto uri = "otpauth-migration://offline?data=" +
                             UriHelper::PercentEncode(Base64::Encode(payload));

            TwoFactor::MigrationResult result;
            const bool ok = TwoFactor::TryParseMigrationUri(uri, result);

            Check("migration payload parses", ok ? "ok" : "failed", "ok");
            Check("migration account count", std::to_string(result.entries.size()), "1");

            if (result.entries.empty())
                return;

            const auto& entry = result.entries[0];
            Check("migration secret decodes to Base32", entry.GetSecretBase32(), "JBSWY3DPEHPK3PXP");
            Check("migration issuer", entry.GetIssuer(), "Example");
            Check("migration account", entry.GetAccount(), "alice@google.com");
            Check("migration digits", std::to_string(entry.GetDigits()), "6");
            Check("migration type is TOTP",
                  entry.GetType() == TwoFactor::OtpType::TOTP ? "totp" : "hotp", "totp");

            // The imported entry must generate the same codes as the equivalent otpauth:// URI.
            TwoFactor::TwoFactorEntry direct;
            TwoFactor::TwoFactorEntry::TryFromUri(
                "otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example", direct);

            std::string fromMigration, fromUri;
            const bool bothGenerated = entry.TryGenerateCode(1234567890LL, fromMigration) &&
                                       direct.TryGenerateCode(1234567890LL, fromUri);
            Check("migration entry matches equivalent URI",
                  bothGenerated ? fromMigration : std::string("<failed>"), fromUri);

            TwoFactor::MigrationResult rejected;
            Check("non-migration URI rejected",
                  TwoFactor::TryParseMigrationUri("otpauth://totp/x?secret=AA", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
            Check("truncated payload rejected",
                  TwoFactor::TryParseMigrationUri("otpauth-migration://offline?data=CjEKCg", rejected)
                      ? "accepted" : "rejected",
                  "rejected");
        }
#pragma endregion
    }

    bool RunSelfTest() {
        totalTests = 0;
        failedTests = 0;

        std::cout << "Clavis cryptography self-test\n\n";

        TestHashes();
        TestHMAC();
        TestBase32();
        TestBase64();
        TestHOTP();
        TestTOTP();
        TestEntryParsing();
        TestFileFormat();
        TestMigrationImport();

        std::cout << "\n" << (totalTests - failedTests) << "/" << totalTests << " vectors passed.\n";

        if (failedTests > 0)
            std::cout << failedTests << " FAILED.\n";

        return failedTests == 0;
    }
}
