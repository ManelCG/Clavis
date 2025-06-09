#include <extensions/RNG.h>

#include <vector>
#include <random>


namespace Clavis::Extensions::RNG {
	std::vector<uint8_t> GetBytes(size_t n) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis;

		auto arr = std::vector<uint32_t>();

		auto passes = (size_t)(ceil((double)n / (double)sizeof(uint32_t)));

		for (auto i = 0; i < passes; i++)
			arr.push_back(dis(gen));

		auto data = (uint8_t*)arr.data();

		std::vector<uint8_t> ret;
		for (int i = 0; i < n; i++)
			ret.push_back(data[i]);

		return ret;
	}

	template<typename T>
	T Get()
	{
		auto size = sizeof(T);
		auto data = GetBytes(size);

		T ret = T();
		auto ptr = (uint8_t*)&ret;
		for (auto i = 0; i < size; i++)
			ptr[i] = data[i];

		return ret;
	}

	uint32_t GetUInt32() {
		return Get<uint32_t>();
	}
	uint16_t GetUInt16() {
		return Get<uint16_t>();
	}
	uint8_t GetUInt8() {
		return Get<uint8_t>();
	}
	int32_t GetInt32() {
		return Get<int32_t>();
	}
	int16_t GetInt16() {
		return Get<int16_t>();
	}
	int8_t GetInt8() {
		return Get<int8_t>();
	}
}