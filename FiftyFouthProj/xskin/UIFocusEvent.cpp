
#include "UIFocusEvent.h"

UIFocusEvent::UIFocusEvent(UIResponser* pSource, EventType nType)
 : UIEvent(pSource, nType, 0)
{

}

UIFocusEvent::~UIFocusEvent()
{

}