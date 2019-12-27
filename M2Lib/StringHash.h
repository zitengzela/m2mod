#pragma once
#include <cstdint>
#include <string>

namespace M2Lib
{
	template <class T>
	uint64_t CalcStringHash(std::basic_string<T> szFileName);

	template <class T>
	std::basic_string<T> NormalizePath(std::basic_string<T> const& path);
}
