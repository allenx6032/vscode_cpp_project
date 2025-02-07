/*
 @ 0xCCCCCCCC
*/

#include "kbase_debugger.h"

#include <Windows.h>

namespace kbase {

bool IsDebuggerPresent()
{
    return ::IsDebuggerPresent();
}

void BreakDebugger()
{
    __debugbreak();
}

}   // namespace kbase
