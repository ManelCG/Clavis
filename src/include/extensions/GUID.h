#pragma once

#include <string>
#include <array>
#include <vector>
#include <cstdint>

constexpr int GUID_BYTES = 16;
constexpr int GUID_BITS = 128;


namespace Clavis::Extensions {
	class GUID {
	public:
		GUID();
		GUID(std::array<uint8_t, GUID_BYTES> data);
		GUID(std::vector<uint8_t> data);

		static GUID NewGuid();
		static GUID FromString(std::string guid);
		static GUID EmptyGuid();

		std::string ToString(bool includeHyphens = true) const;

		operator std::string() const;

		bool operator==(const GUID& other) const;
		bool operator!=(const GUID& other) const;
		bool operator<(const GUID& other) const;
		bool operator>(const GUID& other) const;
		bool operator<=(const GUID& other) const;
		bool operator>=(const GUID& other) const;

	protected:

	private:
		void NormalizeV4GUID();

		std::array<uint8_t, GUID_BYTES> Data;
	};
}
