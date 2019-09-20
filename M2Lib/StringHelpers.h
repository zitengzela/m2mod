#pragma once

#include <string>
#include <algorithm>

namespace std
{
	template <class _Elem, class _Traits>
	inline std::basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL rendl(
		std::basic_ostream<_Elem, _Traits>& _Ostr) { // insert newline and flush stream
		_Ostr.put(_Ostr.widen('\r'));
		_Ostr.put(_Ostr.widen('\n'));
		_Ostr.flush();
		return _Ostr;
	}
}

namespace  M2Lib {
	namespace StringHelpers
	{
		// trim from start (in place)
		template <class T>
		static void ltrim(std::basic_string<T>& s, std::initializer_list<char> characters = { ' ' }) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](int ch) {
				return std::find(characters.begin(), characters.end(), ch) == characters.end();
			}));
		}

		// trim from end (in place)
		template <class T>
		static void rtrim(std::basic_string<T>& s, std::initializer_list<char> characters = { ' ' }) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [&](int ch) {
				return std::find(characters.begin(), characters.end(), ch) == characters.end();
			}).base(), s.end());
		}

		// trim from both ends (in place)
		template <class T>
		static void trim(std::basic_string<T>& s, std::initializer_list<char> characters = { ' ' }) {
			ltrim(s, characters);
			rtrim(s, characters);
		}

		// trim from start (copying)
		template <class T>
		static std::basic_string<T> ltrim_copy(std::basic_string<T> s, std::initializer_list<char> characters = { ' ' }) {
			ltrim(s, characters);
			return s;
		}

		// trim from end (copying)
		template <class T>
		static std::basic_string<T> rtrim_copy(std::basic_string<T> s, std::initializer_list<char> characters = { ' ' }) {
			rtrim(s, characters);
			return s;
		}

		// trim from both ends (copying)
		template <class T>
		static std::basic_string<T> trim_copy(std::basic_string<T> s, std::initializer_list<char> characters = { ' ' }) {
			trim(s, characters);
			return s;
		}

		std::wstring StringToWString(std::string const& str);
		std::string WStringToString(std::wstring const& str);
	};
}
