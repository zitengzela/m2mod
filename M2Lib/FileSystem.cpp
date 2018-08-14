#include "FileSystem.h"
#include <Windows.h>

template <>
std::basic_string<char> M2Lib::FileSystem<char>::PathSeparator = "\\";

template <>
std::basic_string<wchar_t> M2Lib::FileSystem<wchar_t>::PathSeparator = L"\\";

template <class T>
std::basic_string<T> M2Lib::FileSystem<T>::preparePath(std::basic_string<T> const& path)
{
	auto copy = path;

	size_t pos = copy.find('/');
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		copy.replace(pos, 1,  PathSeparator);
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
	path = preparePath(path);
	auto lastSlash = path.find_last_of('\\');
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
	path = preparePath(path);
	auto lastSlash = path.find_last_of('\\');
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
	path = preparePath(path);
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
	path = preparePath(path);
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
