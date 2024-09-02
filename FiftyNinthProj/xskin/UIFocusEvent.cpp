#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "UIFocusEvent.h"

UIFocusEvent::UIFocusEvent(UIResponser* pSource, EventType nType)
 : UIEvent(pSource, nType, 0)
{

}

UIFocusEvent::~UIFocusEvent()
{

}