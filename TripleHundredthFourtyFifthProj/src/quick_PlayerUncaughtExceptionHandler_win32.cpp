
#include "quick_PlayerStdafx.h"
#include "quick_PlayerUncaughtExceptionHandler.h"
#include "cocos_CCDirector.h"

LONG WINAPI handleException(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
	auto engine = cocos2d::ScriptEngineManager::getInstance()->getScriptEngine();;
	if (engine)
	{
		engine->handleAssert("UncaughtException", 0, 0, 0);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

void InstallUncaughtExceptionHandler()
{
	SetUnhandledExceptionFilter(handleException);
}