#pragma once

#include "BaseTypes.h"
#include <string>

namespace M2Lib
{
	template <class T>
	class FileSystem
	{
		static T PathSeparator;

	public:
		static std::basic_string<T> NormalizePath(std::basic_string<T> const& path);
		static std::basic_string<T> Combine(T const* Path, ...);

		static std::basic_string<T> GetCurrentPath();
		static std::basic_string<T> GetParentDirectory(std::basic_string<T> path);
		static std::basic_string<T> GetBaseName(std::basic_string<T> path);
		static std::basic_string<T> GetFileName(std::basic_string<T> path);
		static std::basic_string<T> GetExtension(std::basic_string<T> path);
		static bool IsDirectory(std::basic_string<T> const& path);
		static bool IsFile(std::basic_string<T> const& path);
		static bool CreateDirectory(std::basic_string<T> const& path);
	};

	using FileSystemA = FileSystem<char>;
	using FileSystemW = FileSystem<wchar_t>;
}
