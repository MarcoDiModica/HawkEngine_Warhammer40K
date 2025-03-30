#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdexcept>

#include "Log.h"
#include "App.h"

#define BUFFER_SIZE 14096

bool IsValidFormatString(const char* format)
{
	if (format == nullptr)
		return false;

	if (format[0] == '\0')
		return false;

	return true;
}

void Log(const char file[], int line, LogType type, const char* format, ...)
{
	if (file == nullptr || format == nullptr)
		return;

	static char tmpString1[BUFFER_SIZE];
	static char tmpString2[BUFFER_SIZE];
	va_list ap;

	try
	{
		tmpString1[0] = '\0';
		tmpString2[0] = '\0';

		if (!IsValidFormatString(format))
		{
			strcpy_s(tmpString1, BUFFER_SIZE, "<invalid format string>");
		}
		else
		{
			va_start(ap, format);

			int requiredSize = _vscprintf(format, ap) + 1;

			if (requiredSize > BUFFER_SIZE)
			{
				va_end(ap);
				va_start(ap, format);
				int result = vsprintf_s(tmpString1, BUFFER_SIZE, format, ap);
				if (result < 0)
				{
					strcpy_s(tmpString1, BUFFER_SIZE, "<format error>");
				}
				strcat_s(tmpString1, BUFFER_SIZE, " [TRUNCATED]");
			}
			else
			{
				int result = vsprintf_s(tmpString1, BUFFER_SIZE, format, ap);
				if (result < 0)
				{
					strcpy_s(tmpString1, BUFFER_SIZE, "<format error>");
				}
			}

			va_end(ap);
		}

		int fileNameLength = 0;
		if (file != nullptr)
		{
			fileNameLength = strnlen(file, 255);
		}

		if (fileNameLength > 0 && line > 0)
		{
			sprintf_s(tmpString2, BUFFER_SIZE, "\n%s(%d) : %s", file, line, tmpString1);
		}
		else
		{
			sprintf_s(tmpString2, BUFFER_SIZE, "\n<unknown>: %s", tmpString1);
		}

		OutputDebugStringA(tmpString2);

		if (Application != nullptr)
		{
			sprintf_s(tmpString2, BUFFER_SIZE, "%s", tmpString1);
			Application->AddLog(type, tmpString2);
		}
	}
	catch (const std::exception& e)
	{
		char errorMsg[BUFFER_SIZE];
		sprintf_s(errorMsg, BUFFER_SIZE, "\nLOG ERROR: Exception in logging system: %s", e.what());
		OutputDebugStringA(errorMsg);

		if (Application != nullptr)
		{
			Application->AddLog(LogType::LOG_ERROR, "Error in logging system");
		}
	}
}