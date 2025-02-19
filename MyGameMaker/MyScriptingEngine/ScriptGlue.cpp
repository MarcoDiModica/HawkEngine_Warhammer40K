#include <mono/jit/jit.h>

#include "ScriptGlue.h"

#include "../MyGameEditor/Log.h"

namespace HawkEngine
{
	static void DebugLog(const char* message)
	{
		LOG(LogType::LOG_C_SHARP, message);
	}

	void ScriptGlue::RegisterComponents()
	{
		
	}

	void ScriptGlue::RegisterFunctions()
	{
		mono_add_internal_call("HawkEngine.Utils::Log", DebugLog);
	}
}