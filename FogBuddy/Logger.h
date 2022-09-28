#pragma once
#include<string>
#include <stdio.h>

#define LOG_DEBUG(fmt,...) ((*Logger::getLogLevel() >= Logger::DEBUG) ? printf(fmt, __VA_ARGS__) : NULL)
#define LOG_INFO(fmt,...) ((*Logger::getLogLevel() >= Logger::INFO) ? printf(fmt, __VA_ARGS__) : NULL)

namespace Logger {
	enum Level {
		NONE	= 0,
		INFO	= 1,
		DEBUG	= 2
	};
	Level* getLogLevel();
}
