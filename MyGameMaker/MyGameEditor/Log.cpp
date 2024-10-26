#include <windows.h>
#include <stdio.h>
#include <vector>
#include <tchar.h>
#include "Log.h"
#include "App.h"


#define BUFFER_SIZE 4096


void Log(const char file[], int line, LogType type, const char* format, ...)
{
	static char tmpString1[BUFFER_SIZE];
	static char tmpString2[BUFFER_SIZE];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmpString1, BUFFER_SIZE, format, ap);
	va_end(ap);

	sprintf_s(tmpString2, BUFFER_SIZE, "\n%s(%d) : %s", file, line, tmpString1);
	OutputDebugStringA(tmpString2);

	// Log to Editor Console
	if (Application != nullptr)
	{
		sprintf_s(tmpString2, BUFFER_SIZE, "%s", tmpString1);
		Application->AddLog(type, tmpString2);
	}
}