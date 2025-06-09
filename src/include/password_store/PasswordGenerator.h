#pragma once
#include <string>

namespace Clavis {
    class PasswordGenerator {
    public:
        typedef struct GeneratorSettings {
            int length;
            bool lowercase;
            bool uppercase;
            bool numerals;
            bool symbols;
            bool pronounceable;
        } GeneratorSettings;

        static GeneratorSettings GetDefaultSettings();
        static std::string GeneratePassword(const GeneratorSettings& settings);

    protected:

    private:
        static std::string GenerateNormal(const GeneratorSettings& settings);
        static std::string GeneratePronounceable(const GeneratorSettings& settings);

        // Returns securely generated integer in range [a, b]
        static int SecureRandRange(int a, int b);
    };
}
