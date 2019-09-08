#include "FileSystem.h"

template <>
char M2Lib::FileSystem<char>::PathSeparator = '\\';

template <>
wchar_t M2Lib::FileSystem<wchar_t>::PathSeparator = L'\\';

template <class T>
std::basic_string<T> M2Lib::FileSystem<T>::NormalizePath(std::basic_string<T> const& path)
{
	auto copy = path;

	size_t pos = copy.find('/');
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		copy.replace(pos, 1, std::basic_string<T>({ PathSeparator }));
		// Get the next occurrence from the current position
		pos = copy.find('/', pos + 1);
	}

	return copy;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::NormalizePath(std::basic_string<char> const& path);

template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::NormalizePath(std::basic_string<wchar_t> const& path);
