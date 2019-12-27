#pragma once

#include "BaseTypes.h"
#include <list>
#include <map>

namespace M2Lib
{
	enum LogLevel
	{
		LOG_NONE = 0,
		LOG_INFO = 1,
		LOG_WARNING = 2,
		LOG_ERROR = 4,
		LOG_CUSTOM = 8,

		LOG_ALL_DEFAULT = LOG_INFO | LOG_WARNING | LOG_ERROR,
		LOG_ALL = LOG_INFO | LOG_WARNING | LOG_ERROR | LOG_CUSTOM,
	};

	typedef void(__stdcall* LoggerCallback)(uint8_t LogLevel, wchar_t const*);

	class Logger
	{
		Logger() = default;

		std::map<uint8_t, std::list<LoggerCallback>> AttachedCallbacks;

		void Log(int LogLevel, wchar_t const* format, va_list args);

	public:
		static Logger& getInstance()
		{
			static Logger l;
			return l;
		}

		void AttachCallback(uint8_t logLevel, LoggerCallback callback);
		void DetachCallback(uint8_t logLevel, LoggerCallback callback);

		void LogInfo(wchar_t const* format, ...);
		void LogError(wchar_t const* format, ...);
		void LogWarning(wchar_t const* format, ...);
		void LogCustom(wchar_t const* format, ...);
	};

	M2LIB_API void __cdecl AttachLoggerCallback(uint8_t logLevel, LoggerCallback callback);
	M2LIB_API void __cdecl DetachLoggerCallback(uint8_t logLevel, LoggerCallback callback);
}

#define sLogger M2Lib::Logger::getInstance()
