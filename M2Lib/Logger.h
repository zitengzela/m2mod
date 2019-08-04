#pragma once

#include <string>
#include <list>

namespace M2Lib
{
	enum LogLevel
	{
		LOG_INFO = 1,
		LOG_ERROR = 2,
		LOG_WARNING = 3,
	};

	class Logger
	{
	public:
		typedef void(__stdcall* LoggerCallback)(int LogLevel, char const*);

	private:
		Logger() = default;

		std::list<LoggerCallback> AttachedCallbacks;

		void Log(int LogLevel, char const* format, va_list args);

	public:
		static Logger & getInstance()
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
}

#define sLogger M2Lib::Logger::getInstance()
