#pragma once

#include <string>

namespace M2Lib
{
	template <class T>
	class FileSystem
	{
		static T PathSeparator;

	public:
		static std::basic_string<T> NormalizePath(std::basic_string<T> const& path);
	};

	using FileSystemA = FileSystem<char>;
	using FileSystemW = FileSystem<wchar_t>;
}
