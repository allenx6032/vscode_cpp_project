
#include "UIHierarchyEvent.h"
#include "UIView.h"

UIHierarchyEvent::UIHierarchyEvent(UIView* pSource, EventType nType, UIView* pParent, int nFlags)
	: UIEvent(pSource, nType, nFlags)
{
	m_pParent = pParent;
}

UIHierarchyEvent::~UIHierarchyEvent()
{

}

UIView* UIHierarchyEvent::GetChanged()
{
	return static_cast<UIView*>(GetSource());
}

UIView* UIHierarchyEvent::GetChangedParent()
{
	return m_pParent;
}