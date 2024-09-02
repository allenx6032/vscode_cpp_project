#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "UIWheelEvent.h"

UIWheelEvent::UIWheelEvent(UIResponser* pSource, EventType nType, const UIPoint& point, int zDelta, int nFlags)
 : UIMouseEvent(pSource, nType, point, nFlags)
{
	m_nDelta = zDelta;
}

UIWheelEvent::~UIWheelEvent()
{

}

int UIWheelEvent::GetDelta() const
{
	return m_nDelta;
}