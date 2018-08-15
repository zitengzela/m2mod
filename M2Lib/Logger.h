#pragma once

#include <string>
#include <list>

namespace M2Lib
{
	typedef void(__stdcall *LoggerCallback)(char const*);

	class Logger
	{
	private:
		Logger() {}

		std::list<LoggerCallback> AttachedCallbacks;

	public:
		static Logger & getInstance()
		{
			static Logger l;
			return l;
		}

		void AttachCallback(LoggerCallback callback);
		void Remove(LoggerCallback callback);
		void Log(char const* format, ...);
	};
}

#define sLogger M2Lib::Logger::getInstance()
