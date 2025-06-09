
using namespace std::literals::string_view_literals;

#define LANGUAGES \
_LANG_(ENG, "English") \
_LANG_(ESP, "Español") \
_LANG_(VAL, "Valencià") \


namespace Clavis::Language {
	// Constexpr Map
	// This is a helper struct to imitate map functionality in a constexpr way.
	// Values fetched with 'at' at compiletime are resolved at compiletime.
	// Values fetched at runtime are fetched iteratively as if the map was an array (which it is)
	template <typename Key, typename Value, std::size_t Size>
	struct Map {
		std::array<std::pair<Key, Value>, Size> data;

		[[nodiscard]] constexpr Value at(const Key& key) const {
            for(auto& v : data)
                if (v.first == key)
                    return v.second;

            return data[0].second;
		}

		[[nodiscard]] constexpr Value operator[](const Key& key) const {
			return at(key);
		}

	};


	// Magic enum magic to define the same enum in many different ways without a human redefining the values multiple times
	// Also helps us count the values in a sane way.
	template <class T>
	constexpr size_t countLengthHelper(const std::initializer_list<T>& list)
	{
		return list.size();
	}
	constexpr size_t NLangsCalcConstexpr()
	{
		#define _LANG_(val, ...) #val,	// Stringify each value of the enum to turn them into an array of strings and then fetch its length
		return countLengthHelper({ LANGUAGES });
		#undef _LANG_
	}

	// Number of values in the enum
	constexpr auto NLanguages = NLangsCalcConstexpr();

	enum class LanguagesEnum
	{
		#define _LANG_(val, ...) val,	// Paste the enum value name to satisfy the enum syntax
		LANGUAGES
		#undef _LANG_
	};

	static constexpr auto LanguagesArray = std::array<LanguagesEnum, NLanguages>
	{
		#define _LANG_(x, ...) LanguagesEnum::x,	// Stringify each value of the enum and turn it into a string view with the syntax "data"sv.
		LANGUAGES
		#undef _LANG_
	};

	static constexpr auto LanguageCodesArray = std::array<std::string_view, NLanguages>
	{
		#define _LANG_(x, ...) #x##sv,	// Stringify each value of the enum and turn it into a string view with the syntax "data"sv.
		LANGUAGES
		#undef _LANG_
	};

	static constexpr auto LanguagesMap = Map<LanguagesEnum, std::string_view, LanguagesArray.size()>{ {{
		#define _LANG_(x, ...) {LanguagesEnum::x, #x##sv },	// Turn each enum value into a kvp with the enum value and the stringview version
		LANGUAGES
		#undef _LANG_
	}} };

	static constexpr auto LanguagesNames = Map<LanguagesEnum, std::string_view, LanguagesArray.size()>{ {{
		#define _LANG_(code, name) {LanguagesEnum::code, name##sv },
			LANGUAGES
		#undef _LANG_
	}}};

	static constexpr auto LanguagesMapInverted = Map<std::string_view, LanguagesEnum, LanguagesArray.size()>{ {{
		#define _LANG_(x, ...) {#x##sv, LanguagesEnum::x},	// Turn each enum value into a kvp with the enum value and the stringview version
			LANGUAGES
		#undef _LANG_
	}}};


	// namespace LanguageNames {
	// 	#define _LANG_(val, str) const std::string val = u8##str;
	// 	LANGUAGES
	// 	#undef _LANG_
	// }

	//static constexpr auto LanguageNames = Map<LanguagesEnum, std::string_view, LanguagesArray.size()>{ {{
	//	#define _LANG_(val, str) {LanguagesEnum::val, str},
	//	LANGUAGES
	//	#undef _LANG_
	//}} };


	namespace LanguagesStr
	{
		#define _LANG_(val, ...) constexpr std::string_view val = #val##sv;	// Turn each enum value into a stringview in the LanguagesStr namespace
		LANGUAGES
		#undef _LANG_
	}

	typedef Map<std::string_view, std::string_view, LanguagesArray.size()> _I18N_MAP_;


	#ifdef TRANSLATE_CHECK

		// Not constexpr -> evaluation causes error (intended!!)
		inline static int force_compiletime_error(int x) { return x; }

		#if (TRANSLATE_CHECK == 1)
			// Throws a compiler error if any i18n string misses one single language
			inline constexpr bool AssertAllLanguagesCorrectness(Clavis::Language::_I18N_MAP_ map) {
				for (int i = 0; i < map.data.size(); i++) {
					bool found = false;

					for (int j = 0; j < LanguagesArray.size(); j++) {
						if (LanguagesArray[j] == map.data[i].first)
							found = true;
					}

					if (!found)
						force_compiletime_error(0);
				}

				return true;
			}
			#define _COMPILETIME_FUNC_CHECK AssertAllLanguagesCorrectness
		#else
			#define __STR_HELPER_CLAVIS_TRANSLATION_ENGINE__(x) #x
			#define __STRINGIFIER_CLAVIS_TRANSLATION_ENGINE__(x) __STR_HELPER_CLAVIS_TRANSLATION_ENGINE__(x)

			// Throws a compiler error if any i18n string misses the provided language
			inline constexpr bool AssertLanguageCorrectness(Clavis::Language::_I18N_MAP_ map) {
				for (int i = 0; i < map.data.size(); i++) {
					if (map.data[i].first == __STRINGIFIER_CLAVIS_TRANSLATION_ENGINE__(TRANSLATE_CHECK))
						return true;
				}

				force_compiletime_error(0);
			}
			#define _COMPILETIME_FUNC_CHECK AssertLanguageCorrectness
			#undef __STRINGIFIER_CLAVIS_TRANSLATION_ENGINE__
			#undef __STR_HELPER_CLAVIS_TRANSLATION_ENGINE__
		#endif

		#define _COMPILETIME_CHECK(name) constexpr auto name##_DUMMYVALUE = _COMPILETIME_FUNC_CHECK(name);
	#else
		#define _COMPILETIME_CHECK(...)
	#endif

	// Use this macro to define a translatable resource
	#define _(name, ...) static constexpr auto name = Clavis::Language::_I18N_MAP_ {{{ __VA_ARGS__ }}};  _COMPILETIME_CHECK(name)

}