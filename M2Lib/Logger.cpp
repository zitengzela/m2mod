#include "Logger.h"
#include <Windows.h>

void M2Lib::Logger::AttachCallback(LoggerCallback callback)
{
	AttachedCallbacks.push_back(callback);
}

void M2Lib::Logger::Remove(LoggerCallback callback)
{
	AttachedCallbacks.remove(callback);
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

void M2Lib::Logger::Log(int LogLevel, wchar_t const* format, va_list args)
{
	wchar_t text[4096];

	vswprintf_s(text, sizeof(text) / sizeof(text[0]), format, args);

	for (auto callback : AttachedCallbacks)
		callback(LogLevel, text);
}

void M2Lib::AttachLoggerCallback(M2Lib::LoggerCallback callback)
{
	sLogger.AttachCallback(callback);
}
