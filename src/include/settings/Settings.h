#pragma once

#include <string>
#include <settings/SettingsDefinitions.h>

#include <error/ClavisError.h>

namespace Clavis::Settings {

    bool SettingsInitialize();

	bool SetValue(const std::string& key, const std::string& val);
	bool TryGetValue(const std::string& key, std::string& out);
	std::string GetValue(const std::string& key);

	bool HasValue(const std::string& key);

	bool TryGetDefaultValue(const std::string& key, std::string& out);
	std::string GetDefaultValue(const std::string& key);

	std::string GetAndSetDefaultValue(const std::string& key);
	std::string Resolve(const std::string& key);

	template <typename T>
	bool TryGetAsValue(const std::string& key, T& out);

	template<typename T>
	T GetAsValue(const std::string& key) {
		T ret;
		if (!TryGetAsValue(key, ret))
			RaiseClavisError("Unable to get or convert the value of ", key, " to the requested type.");

		return ret;
	}
}