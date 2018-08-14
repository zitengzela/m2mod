#pragma once

typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned int UInt32;
typedef signed int SInt32;
typedef float Float32;

typedef char Char8;
typedef wchar_t Char16;

#ifndef MakeFourCC
# define MakeFourCC(ch0, ch1, ch2, ch3) ((UInt32)(UInt8)(ch0) | ((UInt32)(UInt8)(ch1) << 8) | ((UInt32)(UInt8)(ch2) << 16) | ((UInt32)(UInt8)(ch3) << 24 ))
#endif

#define REVERSE_CC(x) \
	 ((x & 0xFF) << 24 | ((x >> 8) & 0xFF) << 16 | ((x >> 16) & 0xFF) << 8 | (x >> 24) & 0xFF)

#define ASSERT_SIZE(x, y) \
	static_assert(sizeof(x) == y, #x" size does not match client size")

#define MAKE_VERSION(major, minor) (major << 16 | minor)
