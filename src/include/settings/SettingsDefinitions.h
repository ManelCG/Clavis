#pragma once

// This file contains the definitions of each of the individual settings, along with their default values

#include <vector>

#include <settings/SettingImpl.h>
#include <language/Language.h>

namespace Clavis::Settings {
	__STRING_SETTING__(PASSWORD_STORE_PATH);

	__BOOL_SETTING__(IS_FIRST_RUN);

	__BOOL_SETTING__(DO_USE_DARK_THEME);
	__BOOL_SETTING__(FORCE_CLAVIS_STYLE);
	__BOOL_SETTING__(DISABLE_SHADOWS);

	__BOOL_SETTING__(FILTER_CASE_SENSITIVE);

	__BOOL_SETTING__(SHOW_HIDDEN_FILES);

	#pragma region Password Generator Settings
	__INT_SETTING__(PASSWORD_GENERATOR_DEFAULT_LENGTH);
	__BOOL_SETTING__(PASSWORD_GENERATOR_USE_LOWERCASE);
	__BOOL_SETTING__(PASSWORD_GENERATOR_USE_UPPERCASE);
	__BOOL_SETTING__(PASSWORD_GENERATOR_USE_NUMERALS);
	__BOOL_SETTING__(PASSWORD_GENERATOR_USE_SYMBOLS);
	__BOOL_SETTING__(PASSWORD_GENERATOR_PRONOUNCEABLE);
	#pragma endregion

	__ENUM_SETTING__(WINDOW_DECORATIONS,
		__SETVAL__(CLAVIS_CSD);
		__SETVAL__(GTK_CSD);
		__SETVAL__(FORCE_NO_CSD);
	);

	__ENUM_SETTING__(CLAVIS_LANGUAGE);

#ifdef __WINDOWS__
	__STRING_SETTING__(CLAVIS_FONT);
	__STRING_SETTING__(CLAVIS_THEME);
#endif

#ifdef __DEBUG__
	__BOOL_SETTING__(RUN_GTK_CSS_INSPECTOR);
#endif

	inline const std::vector<std::pair<std::string, std::string>> DefaultSettings();

}

#undef __CLAVIS_SETTING__
#undef __DEBUG_SETTING__