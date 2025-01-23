#ifndef __LOG_H__
#define __LOG_H__
#pragma once

#include <string>
#include "UIConsole.h"

enum class LogType
{
	LOG_INFO,
	LOG_ASSIMP,
	LOG_AUDIO,

	LOG_OK,
	LOG_WARNING,
	LOG_ERROR
};

struct LogInfo
{
	LogType type;
	std::string message;
};

#define LOG(type, format, ...) Log(__FILE__, __LINE__, type, format, ## __VA_ARGS__)

void Log(const char file[], int line, LogType type, const char* format, ...);


#endif  // !__LOG_H__