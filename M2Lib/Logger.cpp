#include "Logger.h"
#include <Windows.h>

void M2Lib::Logger::AttachCallback(uint8_t logLevel, LoggerCallback callback)
{
	for (auto level : { LOG_INFO, LOG_ERROR, LOG_WARNING, LOG_CUSTOM })
		if (logLevel & level)
			AttachedCallbacks[level].push_back(callback);
}

void M2Lib::Logger::DetachCallback(uint8_t logLevel, LoggerCallback callback)
{
	for (auto level : { LOG_INFO, LOG_ERROR, LOG_WARNING, LOG_CUSTOM })
	{
		auto itr = AttachedCallbacks.find(level);
		if (itr == AttachedCallbacks.end())
			continue;

		itr->second.remove(callback);
	}
}

void M2Lib::Logger::LogInfo(wchar_t const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_INFO, format, args);
	va_end(args);
}

void M2Lib::Logger::LogError(wchar_t const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_ERROR, format, args);
	va_end(args);
}

void M2Lib::Logger::LogWarning(wchar_t const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_WARNING, format, args);
	va_end(args);
}

void M2Lib::Logger::LogCustom(wchar_t const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_CUSTOM, format, args);
	va_end(args);
}

void M2Lib::Logger::Log(int LogLevel, wchar_t const* format, va_list args)
{
	auto itr = AttachedCallbacks.find(LogLevel);
	if (itr == AttachedCallbacks.end())
		return;

	wchar_t text[4096];

	vswprintf_s(text, sizeof(text) / sizeof(text[0]), format, args);

	for (auto callback : itr->second)
		callback(LogLevel, text);
}

void M2Lib::AttachLoggerCallback(uint8_t logLevel, LoggerCallback callback)
{
	sLogger.AttachCallback(logLevel, callback);
}

void M2Lib::DetachLoggerCallback(uint8_t logLevel, LoggerCallback callback)
{
	sLogger.DetachCallback(logLevel, callback);
}
