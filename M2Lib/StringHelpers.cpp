#include "StringHelpers.h"
#include <locale>
#include <codecvt>

std::wstring M2Lib::StringHelpers::StringToWString(std::string const& str)
{
	try
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
	}
	catch (std::runtime_error&)
	{
		throw std::runtime_error("Failed to convert string to wstring");
	}
}

std::string M2Lib::StringHelpers::WStringToString(std::wstring const& str)
{
	try
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
	}
	catch (std::runtime_error&)
	{
		throw std::runtime_error("Failed to convert wstring to string");
	}
}
