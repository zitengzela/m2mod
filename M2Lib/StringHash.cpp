#include "StringHash.h"
#include "lookup.h"
#include "FileSystem.h"
#include <algorithm>

uint64_t CalcJenkinsHash(const void* data, size_t nLength)
{
	uint32_t dwHashHigh = 0;
	uint32_t dwHashLow = 0;

	hashlittle2(data, nLength, &dwHashHigh, &dwHashLow);
	return ((uint64_t)dwHashHigh << 0x20) | dwHashLow;
}

template <class T>
uint64_t M2Lib::CalcStringHash(std::basic_string<T> path)
{
	auto pathCopy = FileSystem<T>::NormalizePath(path);
	std::transform(pathCopy.begin(), pathCopy.end(), pathCopy.begin(), [](auto c) {return ::toupper(c); });

	return CalcJenkinsHash(pathCopy.c_str(), pathCopy.length() * sizeof(T));
}
