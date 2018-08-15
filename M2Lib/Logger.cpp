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

void M2Lib::Logger::Log(char const* format, ...)
{
	char text[4096];

	time_t rawtime;
	struct tm *info;
	char buffer[80];

	time(&rawtime);

	info = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "[%Y-%m-%d %X] ", info);

	memcpy(text, buffer, strlen(buffer));

	va_list args;
	va_start(args, format);
	vsprintf(&text[strlen(buffer)], format, args);
	va_end(args);

	for (auto callback : AttachedCallbacks)
		callback(text);
}
