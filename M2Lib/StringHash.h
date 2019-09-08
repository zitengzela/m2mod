#pragma once
#include <cstdint>
#include <string>

namespace M2Lib
{
	template <class T>
	uint64_t CalcStringHash(std::basic_string<T> szFileName);

	template
	uint64_t CalcStringHash(std::basic_string<char> szFileName);

	template
	uint64_t CalcStringHash(std::basic_string<wchar_t> szFileName);
}
