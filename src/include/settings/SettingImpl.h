#pragma once

// This file contains the definition of the Setting class and its derivations

#include <string>
#include <vector>
#include <extensions/GUID.h>

namespace Clavis::Settings {
	enum class SettingType {
		STRING,
		BOOL,
		INTEGER,
		GUID,
		ENUM
	};

	class _SettingBaseClass {
	public:
		constexpr _SettingBaseClass(std::string_view name, SettingType type) : Name(name), Type(type) {}

		constexpr operator std::string_view() const { return Name; }
		constexpr operator const char* () const { return Name.data(); }

		std::string GetName() const { return std::string(Name); }
		constexpr SettingType GetType() const { return Type; }

		operator std::string() const;
		bool HasValue() const;

	protected:
		std::string_view Name;
		SettingType Type;
	};

	// These classes need to be separated because the baseclass::HasValue depends on Settings.h so it must be in a .cpp file to avoid circular #include, but we do not want to specialize it as a template function for each type.
	template <typename T>
	class _SettingBaseClassT : public _SettingBaseClass {
	public:
		constexpr _SettingBaseClassT(std::string_view name, SettingType type) : _SettingBaseClass(name, type) {}

		T GetValue() const;
		void SetValue(T value) const;
		bool TryGetValue(T& out) const;

		static std::pair<std::string, std::string> Kvp(std::string_view setting, T value);
		static std::pair<std::string, std::string> Kvp(std::string_view setting, std::string_view value);
		static std::pair<std::string, std::string> Kvp(std::string_view setting, const char* value);
	};


	using Setting = _SettingBaseClass*;

	using StringSetting = _SettingBaseClassT<std::string>;
	using BoolSetting   = _SettingBaseClassT<bool>;
	using IntSetting    = _SettingBaseClassT<int32_t>;
	using GuidSetting   = _SettingBaseClassT<Extensions::GUID>;
}

#define __SETVAL__(val) constexpr static std::string_view val = #val
#define __ENUM_SETTING__(setting, ...) \
	namespace _SettingClasses {\
		class setting##_IMPLCLASS : public StringSetting { \
		public: \
			constexpr setting##_IMPLCLASS(std::string_view name) : StringSetting(name, SettingType::ENUM) {} \
			__VA_ARGS__ \
		}; \
	} \
	constexpr _SettingClasses::setting##_IMPLCLASS setting(#setting)


#define __STRING_SETTING__(setting) constexpr static StringSetting setting(#setting, SettingType::STRING)
#define __BOOL_SETTING__(setting)   constexpr static BoolSetting   setting(#setting, SettingType::BOOL)
#define __INT_SETTING__(setting)    constexpr static IntSetting    setting(#setting, SettingType::INTEGER)
#define __GUID_SETTING__(setting)   constexpr static GuidSetting   setting(#setting, SettingType::GUID)

