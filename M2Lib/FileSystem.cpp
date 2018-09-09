#include "FileSystem.h"
#include <Windows.h>
#include <ShlObj_core.h>

#ifdef CreateDirectory
# undef CreateDirectory
#endif

template <>
char M2Lib::FileSystem<char>::PathSeparator = '\\';

template <>
wchar_t M2Lib::FileSystem<wchar_t>::PathSeparator = L'\\';

template <class T>
std::basic_string<T> M2Lib::FileSystem<T>::Combine(T const* Path, ...)
{
	const T * element;
	va_list argList;

	auto result = FixPath(Path);

	va_start(argList, Path);
	while ((element = va_arg(argList, const T *)) != NULL)
	{
		if (result.find_last_of(PathSeparator) != result.length() - 1)
			result += PathSeparator;

		result += element;

	}
	va_end(argList);

	return result;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::Combine(char const* Path, ...);
template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::Combine(wchar_t const* Path, ...);

template <class T>
std::basic_string<T> M2Lib::FileSystem<T>::FixPath(std::basic_string<T> const& path)
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

template <>
std::basic_string<char> M2Lib::FileSystem<char>::GetCurrentPath()
{
	char Directory[1024];
	GetCurrentDirectoryA(sizeof(Directory), Directory);

	return std::string(Directory);
}

template<class T>
std::basic_string<T> M2Lib::FileSystem<T>::GetParentDirectory(std::basic_string<T> path)
{
	path = FixPath(path);
	auto lastSlash = path.find_last_of(PathSeparator);
	if (lastSlash != std::string::npos)
		return path.substr(0, lastSlash);

	return path;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::GetParentDirectory(std::basic_string<char> path);
template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::GetParentDirectory(std::basic_string<wchar_t> path);

template<class T>
std::basic_string<T> M2Lib::FileSystem<T>::GetBaseName(std::basic_string<T> path)
{
	path = FixPath(path);
	auto lastSlash = path.find_last_of(PathSeparator);
	if (lastSlash != std::string::npos)
		return path.substr(lastSlash + 1);

	return path;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::GetBaseName(std::basic_string<char> path);
template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::GetBaseName(std::basic_string<wchar_t> path);

template<class T>
std::basic_string<T> M2Lib::FileSystem<T>::GetFileName(std::basic_string<T> path)
{
	path = FixPath(path);
	auto BaseName = GetBaseName(path);
	auto lastDot = BaseName.find_last_of('.');
	if (lastDot != std::string::npos)
		return BaseName.substr(0, lastDot);

	return BaseName;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::GetFileName(std::basic_string<char> path);
template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::GetFileName(std::basic_string<wchar_t> path);

template<class T>
std::basic_string<T> M2Lib::FileSystem<T>::GetExtension(std::basic_string<T> path)
{
	path = FixPath(path);
	auto BaseName = GetBaseName(path);
	auto lastDot = BaseName.find_last_of('.');
	if (lastDot != std::string::npos)
		return BaseName.substr(lastDot);

	return BaseName;
}

template
std::basic_string<char> M2Lib::FileSystem<char>::GetExtension(std::basic_string<char> path);
template
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::GetExtension(std::basic_string<wchar_t> path);

template<>
bool M2Lib::FileSystem<char>::IsDirectory(std::basic_string<char> const & path)
{
	DWORD dwAttrib = GetFileAttributesA(path.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

template<>
bool M2Lib::FileSystem<wchar_t>::IsDirectory(std::basic_string<wchar_t> const & path)
{
	DWORD dwAttrib = GetFileAttributesW(path.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

template<>
bool M2Lib::FileSystem<char>::IsFile(std::basic_string<char> const & path)
{
	DWORD dwAttrib = GetFileAttributesA(path.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

template<>
bool M2Lib::FileSystem<wchar_t>::IsFile(std::basic_string<wchar_t> const & path)
{
	DWORD dwAttrib = GetFileAttributesW(path.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

template<>
bool M2Lib::FileSystem<char>::CreateDirectory(std::basic_string<char> const & path)
{
	return SHCreateDirectoryExA(NULL, path.c_str(), NULL) == ERROR_SUCCESS;
}

template<>
bool M2Lib::FileSystem<wchar_t>::CreateDirectory(std::basic_string<wchar_t> const & path)
{
	return SHCreateDirectoryExW(NULL, path.c_str(), NULL) == ERROR_SUCCESS;
}
