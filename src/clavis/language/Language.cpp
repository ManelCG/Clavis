#include <language/Language.h>

#include <settings/Settings.h>

#include <regex>

namespace Clavis::Language {
	Glib::ustring GetResourceString(_I18N_MAP_ strings, std::vector<std::string> arguments)
	{
		auto lang = Settings::CLAVIS_LANGUAGE.GetValue();

		std::string s(strings.at(lang));

		for (int i = 0; i < arguments.size(); i++) {
			std::string interpolation_token = "\\{" + std::to_string(i) + "\\}";

			s = std::regex_replace(s, std::regex(interpolation_token), arguments[i]);
		}

		return s;
	}

	std::string GetLanguageCode(const LanguagesEnum language) {
		return std::string(LanguagesMap[language]);
	}

	std::string GetLanguageName(const LanguagesEnum language) {
		return std::string(LanguagesNames[language]);
	}

	std::pair<std::string, std::string> GetLanguageData(const LanguagesEnum language) {
		return {GetLanguageCode(language), GetLanguageName(language)};
	}

	std::vector<LanguagesEnum> GetAllLanguages() {
		std::vector<LanguagesEnum> result;

		for (const auto& lang : LanguagesArray) {
			result.push_back(lang);
		}

		return result;
	}

	LanguagesEnum GetLanguageFromCode(const std::string &code) {
		return LanguagesMapInverted[code];
	}



}