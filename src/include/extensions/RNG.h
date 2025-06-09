#pragma once

#include <stdint.h>
#include <vector>

namespace Clavis::Extensions::RNG {
	std::vector<uint8_t> GetBytes(size_t n);

	uint32_t GetUInt32();
	uint16_t GetUInt16();
	uint8_t GetUInt8();

	int32_t GetInt32();
	int16_t GetInt16();
	int8_t GetInt8();
}