#include <extensions/GUID.h>

#include <extensions/RNG.h>
#include <error/ClavisError.h>
#include <language/Language.h>

#include <sstream>
#include <iomanip>

#include <cstring>

#include <regex>

namespace Clavis::Extensions {
	GUID::GUID() {
		Data.fill(0);
	}

	GUID::GUID(std::array<uint8_t, GUID_BYTES> data) {
		std::memcpy(Data.data(), data.data(), GUID_BYTES);
	}

	GUID::GUID(std::vector<uint8_t> data) {
		if (data.size() != GUID_BYTES)
			RaiseClavisError(_(ERROR_GUID_FROM_VECTOR_INCORRECT_SIZE, std::to_string(GUID_BYTES), std::to_string(data.size())));

		std::memcpy(Data.data(), data.data(), GUID_BYTES);

		NormalizeV4GUID();
	}

	void GUID::NormalizeV4GUID() {
		Data[7] &= 0x0f;
		Data[7] |= 0x40;

		Data[9] &= 0x3f;
		Data[9] |= 0x80;
	}

	GUID GUID::NewGuid() {
		return GUID(RNG::GetBytes(GUID_BYTES));
	}

	GUID GUID::EmptyGuid() {
		auto g = GUID();
		g.Data.fill(0);

		return g;
	}

	std::string GUID::ToString(bool includeHyphens) const {
		std::stringstream stream;


		for (int i = 0; i < GUID_BYTES; i++) {
			auto value = Data[i];

			stream << std::hex
				   << std::setfill('0') 
			       << std::setw(2)
				   << (uint32_t)value;

			if (includeHyphens) {
				switch (i) {
				case 3:
				case 5:
				case 7:
				case 9:
					stream << '-';
				default:
				{}
				}
			}
		}

		return stream.str();
	}

	GUID GUID::FromString(std::string guid) {
		auto sNorm = std::regex_replace(guid, std::regex("-"), "");

		if (sNorm.size() != GUID_BYTES*2)
			RaiseClavisError(_(ERROR_GUID_FROM_STRING_INCORRECT_SIZE, guid));


		std::string s;
		uint8_t byte;
		std::vector<uint8_t> data;
		for (int i = 0; i < GUID_BYTES; i++) {
			std::stringstream ss;

			s = "0x";
			s += sNorm[i*2];
			s += sNorm[i*2 + 1];

			byte = (uint8_t) std::stoi(s, nullptr, 16);

			data.push_back(byte);
		}

		return GUID(data);
	}

	GUID::operator std::string() const {
		return ToString();
	}

	bool GUID::operator ==(const GUID& other) const {
		for (int i = 0; i < GUID_BYTES; i++)
			if (other.Data[i] != Data[i])
				return false;

		return true;
	}

	bool GUID::operator != (const GUID& other) const {
		return !operator==(other);
	}

	bool GUID::operator <(const GUID& other) const {
		for (int i = 0; i < GUID_BYTES; i++) {
			if (Data[i] == other.Data[i])
				continue;

			if (Data[i] < other.Data[i])
				return true;
			else
				return false;
		}

		return false;
	}
	bool GUID::operator >(const GUID& other) const {
		for (int i = 0; i < GUID_BYTES; i++) {
			if (Data[i] == other.Data[i])
				continue;

			if (Data[i] > other.Data[i])
				return true;
			else
				return false;
		}

		return false;
	}

	bool GUID::operator <=(const GUID& other) const {
		return operator<(other) || operator==(other);
	}

	bool GUID::operator >=(const GUID& other) const {
		return operator>(other) || operator==(other);
	}
}
