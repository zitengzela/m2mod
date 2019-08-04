#include "Logger.h"
#include <time.h>
#include <Windows.h>

void M2Lib::Logger::AttachCallback(LoggerCallback callback)
{
	AttachedCallbacks.push_back(callback);
}

void M2Lib::Logger::Remove(LoggerCallback callback)
{
	AttachedCallbacks.remove(callback);
}

void M2Lib::Logger::LogInfo(char const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_INFO, format, args);
	va_end(args);
}

void M2Lib::Logger::LogError(char const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_ERROR, format, args);
	va_end(args);
}

void M2Lib::Logger::LogWarning(char const* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(LOG_WARNING, format, args);
	va_end(args);
}

void M2Lib::Logger::Log(int LogLevel, char const* format, va_list args)
{
	char text[4096];

	vsprintf(text, format, args);

	for (auto callback : AttachedCallbacks)
		callback(LogLevel, text);
}
