#pragma once
#include <cstdint>

#ifndef MakeFourCC
# define MakeFourCC(ch0, ch1, ch2, ch3) ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

#define REVERSE_CC(x) \
	 ((x & 0xFF) << 24 | ((x >> 8) & 0xFF) << 16 | ((x >> 16) & 0xFF) << 8 | (x >> 24) & 0xFF)

#define ASSERT_SIZE(x, y) \
	static_assert(sizeof(x) == y, #x" size does not match expected size")

#define MAKE_VERSION(major, minor) (major << 16 | minor)
