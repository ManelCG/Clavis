#include <settings/SettingImpl.h>

#include <settings/Settings.h>

#define __DEFINE_SPECIFICATION__(T, conversionToString) \
	template <> T _SettingBaseClassT<T>::GetValue() const { \
		return Clavis::Settings::GetAsValue<T>(std::string(Name)); \
	} \
	template<> void _SettingBaseClassT<T>::SetValue(T value) const { \
		Clavis::Settings::SetValue(std::string(Name), conversionToString); \
	} \
	template<> bool _SettingBaseClassT<T>::TryGetValue(T& out) const { \
		try { \
			out = GetValue(); \
			return true; \
		} \
		catch(...) { \
			return false; \
		}\
	}\
	template<> std::pair<std::string, std::string> _SettingBaseClassT<T>::Kvp(std::string_view setting, T value)  {\
		return { std::string(setting), conversionToString  }; \
	}\
	template<> std::pair<std::string, std::string> _SettingBaseClassT<T>::Kvp(std::string_view setting, std::string_view value)  {\
		return { std::string(setting), std::string(value)  }; \
	}\
	template<> std::pair<std::string, std::string> _SettingBaseClassT<T>::Kvp(std::string_view setting, const char* value)  {\
		return { std::string(setting), std::string(value)  }; \
	}\


namespace Clavis::Settings {
	_SettingBaseClass::operator std::string() const {
		return std::string(Name);
	}
	bool _SettingBaseClass::HasValue() const {
		return Settings::HasValue(std::string(Name));
	}

	__DEFINE_SPECIFICATION__(std::string,        value)
	__DEFINE_SPECIFICATION__(bool,               value ? "true" : "false")
	__DEFINE_SPECIFICATION__(int32_t,            std::to_string(value))
	__DEFINE_SPECIFICATION__(Extensions::GUID,   value.ToString())

}
