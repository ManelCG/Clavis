#include <settings/Settings.h>

#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>


#include <tools/json.hpp>

using json = nlohmann::json;

#include <system/Extensions.h>
#include <extensions/GUID.h>

namespace Clavis::Settings {
	// Functions only available here
	bool IsSettingsInitialized();
	bool SettingsTerminate();

	void PopulateWithDefaultValues();

	bool UpdateSettingsFile();
	std::filesystem::path GetSettingsFilePath();

	json SettingsToJSON();

	typedef class ClavisSettings {
	public:
		bool isInitialized = false;

		bool StoreInFile(std::filesystem::path path);
		bool ReadFromFile(std::filesystem::path path);

		std::map<std::string, std::string> settings;
	} Settings;

	Settings globalSettings;


	bool SettingsInitialize()
	{
		if (IsSettingsInitialized())
			SettingsTerminate();

		auto folder = Clavis::System::GetClavisConfigFolder();
		if (!System::mkdir_p(folder))
			RaiseClavisError("Unable to create directory: \"", folder.string(), "\". Error code: ", std::to_string(errno), "\n");


		auto file = GetSettingsFilePath();

		if (!Clavis::System::FileExists(file)) {
			PopulateWithDefaultValues();
			globalSettings.StoreInFile(file);
			globalSettings.isInitialized = true;
			return true;
		}

		globalSettings.ReadFromFile(file);
		globalSettings.isInitialized = true;

		PopulateWithDefaultValues();
		globalSettings.StoreInFile(file);

		return false;
	}

	bool IsSettingsInitializedOnDisk() {
		return System::FileExists(GetSettingsFilePath());
	}


	bool SetValue(const std::string& key, const std::string& val) {
		globalSettings.settings[key] = val;

		if (!UpdateSettingsFile())
			RaiseClavisError("Unable to set a value in the settings.\nThe KeyValuePair is: {'", key, "' - '", val, "'}\n");

		return true;
	}

	bool TryGetValue(const std::string& key, std::string& out) {
		if (!HasValue(key))
			return false;

			out= globalSettings.settings[key];
		return true;

	}

	std::string GetValue(const std::string& key) {
		std::string out;
		if (!TryGetValue(key, out))
			RaiseClavisError("Unable to get value for setting: ", key);

		return out;
	}

	bool HasValue(const std::string& key) {
		return globalSettings.settings.find(key) != globalSettings.settings.end();
	}

	bool TryGetDefaultValue(const std::string& key, std::string& out) {
		for (int i = 0; i < DefaultSettings().size(); i++) {
			if (key == DefaultSettings().at(i).first) {
				out = DefaultSettings().at(i).second;
				return true;
			}
		}

		return false;
	}

	std::string GetDefaultValue(const std::string& key) {
		std::string out;
		if (!TryGetDefaultValue(key, out))
			RaiseClavisError("Unable to get default value for setting: ", key, ". Perhaps it is undefined?");

		return out;
	}

	std::string GetAndSetDefaultValue(const std::string& key) {
		auto val = GetDefaultValue(key);
		SetValue(key, val);
		return val;
	}

	std::string Resolve(const std::string& key) {
		std::string val;
		if (!TryGetValue(key, val))
			val = GetAndSetDefaultValue(key);

		return val;
	}

	// Specializations

	template<> // Trivial specialization
	bool TryGetAsValue<std::string>(const std::string& key, std::string& out) {
		return TryGetValue(key, out);
	}

	template<>
	bool TryGetAsValue<bool>(const std::string& key, bool& out) {
		std::string s;
		if (!TryGetValue(key, s))
			return false;

		if (s == "true")
			out = true;
		else if (s == "false")
			out = false;
		else
			return false;

		return true;
	}

	template<>
	bool TryGetAsValue<int32_t>(const std::string& key, int32_t& out) {
		std::string s;
		if (!TryGetValue(key, s))
			return false;

		try {
			out = std::stoi(s);
		}
		catch (...) {
			return false;
		}

		return true;

	}

	template<>
	bool TryGetAsValue<Extensions::GUID>(const std::string& key, Extensions::GUID& out) {
		std::string s;
		if (!TryGetValue(key, s))
			return false;

		try {
			out = Extensions::GUID::FromString(s);
			return true;
		}
		catch (...) {
			return false;
		}
	}

	// "Private" stuff (things only available inside this cpp file, aka the deep implementation)
	void PopulateWithDefaultValues()
	{
		for (int i = 0; i < DefaultSettings().size(); i++)
			if (!HasValue(std::string(DefaultSettings().at(i).first)))
				globalSettings.settings.insert(DefaultSettings().at(i));
	}

	bool UpdateSettingsFile()
	{
		if (!IsSettingsInitialized())
			return false;

		auto file = GetSettingsFilePath();
		return globalSettings.StoreInFile(file);
	}

	std::filesystem::path GetSettingsFilePath() {
		return Clavis::System::GetClavisConfigFolder() / "Settings.json";
	}

	json SettingsToJSON()
	{
		return json(globalSettings.settings);
	}

	bool IsSettingsInitialized()
	{
		return globalSettings.isInitialized;
	}

	bool SettingsTerminate()
	{
		if (!IsSettingsInitialized())
			return false;

		globalSettings.isInitialized = false;
		globalSettings.settings = std::map<std::string, std::string>();
		return true;
	}
	bool ClavisSettings::StoreInFile(std::filesystem::path path)
	{
		std::ofstream file;
		file.open(path.string(), std::ios::trunc | std::ios::out);

		if (!file.good())
			RaiseClavisError("Unable to open settings file for writing");

		file << SettingsToJSON().dump(4);

		return true;
	}
	bool ClavisSettings::ReadFromFile(std::filesystem::path path) {
		json j = System::JSONParseFile(path);

		globalSettings.settings = j.get<std::map<std::string, std::string>>();

		return true;
	}

	const std::vector<std::pair<std::string, std::string>> DefaultSettings() {
		return {
			BoolSetting::Kvp(IS_FIRST_RUN, true),
			BoolSetting::Kvp(DO_USE_DARK_THEME, true),
			BoolSetting::Kvp(DISABLE_SHADOWS, false),
			BoolSetting::Kvp(SHOW_HIDDEN_FILES, false),

			BoolSetting::Kvp(FILTER_CASE_SENSITIVE, false),

			StringSetting::Kvp(CLAVIS_LANGUAGE, Language::LanguagesStr::ENG),

			StringSetting::Kvp(PASSWORD_STORE_PATH, System::GetPasswordStoreDefaultFolder().string()),

			IntSetting::Kvp(PASSWORD_GENERATOR_DEFAULT_LENGTH, 25),
			BoolSetting::Kvp(PASSWORD_GENERATOR_USE_LOWERCASE, true),
			BoolSetting::Kvp(PASSWORD_GENERATOR_USE_UPPERCASE, true),
			BoolSetting::Kvp(PASSWORD_GENERATOR_USE_NUMERALS, true),
			BoolSetting::Kvp(PASSWORD_GENERATOR_USE_SYMBOLS, false),
			BoolSetting::Kvp(PASSWORD_GENERATOR_PRONOUNCEABLE, false),

			#ifdef __LINUX__
			StringSetting::Kvp(WINDOW_DECORATIONS, WINDOW_DECORATIONS.GTK_CSD),
			BoolSetting::Kvp(FORCE_CLAVIS_STYLE, false),
			#elif defined __WINDOWS__
			StringSetting::Kvp(WINDOW_DECORATIONS, WINDOW_DECORATIONS.CLAVIS_CSD),
			BoolSetting::Kvp(FORCE_CLAVIS_STYLE, true),
			#endif

			#ifdef __WINDOWS__
			StringSetting::Kvp(CLAVIS_THEME, "Adwaita"),
			StringSetting::Kvp(CLAVIS_FONT, "Segoe UI 9"),
			#endif

			#ifdef __DEBUG__
			BoolSetting::Kvp(RUN_GTK_CSS_INSPECTOR, false),
			#endif
		};
	}
}

