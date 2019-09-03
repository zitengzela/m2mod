#pragma once

#include "BaseTypes.h"
#include <string>
#include <list>

namespace M2Lib
{
	enum LogLevel
	{
		LOG_INFO = 0,
		LOG_WARNING = 1,
		LOG_ERROR = 2,
	};

	typedef void(__stdcall* LoggerCallback)(int LogLevel, char const*);

	class Logger
	{
		Logger() = default;

		std::list<LoggerCallback> AttachedCallbacks;

		void Log(int LogLevel, char const* format, va_list args);

	public:
		static Logger& getInstance()
		{
			static Logger l;
			return l;
		}

		void AttachCallback(LoggerCallback callback);
		void Remove(LoggerCallback callback);

		void LogInfo(char const* format, ...);
		void LogError(char const* format, ...);
		void LogWarning(char const* format, ...);
	};

	M2LIB_API void __cdecl AttachLoggerCallback(LoggerCallback callback);
}

#define sLogger M2Lib::Logger::getInstance()
