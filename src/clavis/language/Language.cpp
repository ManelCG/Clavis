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


}