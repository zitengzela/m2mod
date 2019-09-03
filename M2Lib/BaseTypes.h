#pragma once
#include <cstdint>

#ifdef M2LIB_EXPORT
# define M2LIB_API extern "C" __declspec(dllexport)
# define M2LIB_API_CLASS __declspec(dllexport)
#else
# define M2LIB_API extern "C" __declspec(dllimport)
# define M2LIB_API_CLASS __declspec(dllimport)
#endif

typedef void* M2LIB_HANDLE;

#ifndef MakeFourCC
# define MakeFourCC(ch0, ch1, ch2, ch3) ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

#define REVERSE_CC(x) \
	 ((x & 0xFF) << 24 | ((x >> 8) & 0xFF) << 16 | ((x >> 16) & 0xFF) << 8 | (x >> 24) & 0xFF)

#define ASSERT_SIZE(x, y) \
	static_assert(sizeof(x) == y, #x" size does not match expected size " #y)

#define MAKE_VERSION(major, minor) (major << 16 | minor)

#if 1
#define m2lib_assert(x) \
	{ if (!(x)) throw std::runtime_error(#x); }
#else
# define m2lib_assert(x) \
	{ assert(x); }
#endif
