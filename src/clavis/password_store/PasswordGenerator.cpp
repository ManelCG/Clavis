#include <password_store/PasswordGenerator.h>

#include <random>
#include <stdexcept>
#include <limits>
#include <cstdint>

#include <settings/Settings.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#else
    #include <fcntl.h>
    #include <unistd.h>
#endif


constexpr std::string_view lowercaseVowels = "aeiou";
constexpr std::string_view uppercaseVowels = "AEIOU";
constexpr std::string_view lowercaseConsonants = "bcdfghjklmnpqrstvwxyz";
constexpr std::string_view uppercaseConsonants = "BCDFGHJKLMNPQRSTVWXYZ";
constexpr std::string_view numerals = "0123456789";
constexpr std::string_view symbols = "!@#$%^&*()_+-=[]{};:,./<>?";

namespace Clavis {
    PasswordGenerator::GeneratorSettings PasswordGenerator::GetDefaultSettings() {
        GeneratorSettings settings;

        settings.length = Settings::PASSWORD_GENERATOR_DEFAULT_LENGTH.GetValue();
        settings.lowercase = Settings::PASSWORD_GENERATOR_USE_LOWERCASE.GetValue();
        settings.uppercase = Settings::PASSWORD_GENERATOR_USE_UPPERCASE.GetValue();
        settings.numerals = Settings::PASSWORD_GENERATOR_USE_NUMERALS.GetValue();
        settings.symbols = Settings::PASSWORD_GENERATOR_USE_SYMBOLS.GetValue();
        settings.pronounceable = Settings::PASSWORD_GENERATOR_PRONOUNCEABLE.GetValue();

        return settings;
    }

    std::string PasswordGenerator::GenerateNormal(const GeneratorSettings& settings) {
        std::string pool;
        std::string password = "";

        if (settings.lowercase) {
            pool += lowercaseVowels;
            pool += lowercaseConsonants;
        }
        if (settings.uppercase) {
            pool += uppercaseVowels;
            pool += uppercaseConsonants;
        }
        if (settings.numerals)
            pool += numerals;

        if (settings.symbols)
            pool += symbols;

        for (int i = 0; i < settings.length; i++) {
            auto index = SecureRandRange(0, pool.size() - 1);
            password += pool[index];
        }

        return password;
    }

    std::string PasswordGenerator::GeneratePronounceable(const GeneratorSettings& settings) {
        std::string consonantsPool;
        std::string vowelsPool;
        std::string password = "";

        if (settings.lowercase) {
            consonantsPool += lowercaseConsonants;
            vowelsPool += lowercaseVowels;
        }
        if (settings.uppercase) {
            consonantsPool += uppercaseConsonants;
            vowelsPool += uppercaseVowels;
        }

        for (int i = 0; i < settings.length; i++) {
            if (i % 2 == 0) {
                const auto index = SecureRandRange(0, consonantsPool.size() - 1);
                password += consonantsPool[index];
            } else {
                const auto index = SecureRandRange(0, vowelsPool.size() - 1);
                password += vowelsPool[index];
            }
        }

        return password;
    }


    std::string PasswordGenerator::GeneratePassword(const GeneratorSettings &settings) {
        if (settings.pronounceable)
            return GeneratePronounceable(settings);

        return GenerateNormal(settings);
    }

    int PasswordGenerator::SecureRandRange(int a, int b) {
        if (a > b)
            std::swap(a, b);

        std::random_device rd;
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rd);
    }


}