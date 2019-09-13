#pragma once

#include "BaseTypes.h"
#include <list>

namespace M2Lib
{
	enum LogLevel
	{
		LOG_INFO = 0,
		LOG_WARNING = 1,
		LOG_ERROR = 2,
	};

	typedef void(__stdcall* LoggerCallback)(int LogLevel, wchar_t const*);

	class Logger
	{
		Logger() = default;

		std::list<LoggerCallback> AttachedCallbacks;

		void Log(int LogLevel, wchar_t const* format, va_list args);

	public:
		static Logger& getInstance()
		{
			static Logger l;
			return l;
		}

		void AttachCallback(LoggerCallback callback);
		void Remove(LoggerCallback callback);

		void LogInfo(wchar_t const* format, ...);
		void LogError(wchar_t const* format, ...);
		void LogWarning(wchar_t const* format, ...);
	};

	M2LIB_API void __cdecl AttachLoggerCallback(LoggerCallback callback);
}

#define sLogger M2Lib::Logger::getInstance()
