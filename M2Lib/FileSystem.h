#pragma once

#include <string>

namespace M2Lib
{
	template <class T>
	class FileSystem
	{
		static std::basic_string<T> PathSeparator;
		static std::basic_string<T> preparePath(std::basic_string<T> const& path);

	public:
		static std::basic_string<T> GetCurrentPath();
		static std::basic_string<T> GetParentDirectory(std::basic_string<T> path);
		static std::basic_string<T> GetBaseName(std::basic_string<T> path);
		static std::basic_string<T> GetFileName(std::basic_string<T> path);
		static std::basic_string<T> GetExtension(std::basic_string<T> path);
		static bool IsDirectory(std::basic_string<T> const& path);
		static bool IsFile(std::basic_string<T> const& path);
	};

	using FileSystemA = FileSystem<char>;
	using FileSystemW = FileSystem<wchar_t>;
}
