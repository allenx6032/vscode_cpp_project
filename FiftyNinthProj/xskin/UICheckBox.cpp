#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "UICheckBox.h"
#include "UIApplication.h"
#include "UIActionEvent.h"

IMPLEMENT_XML_NODE(UICheckBox)

BEGIN_XML_PROPERTY(UICheckBox)
END_XML_PROPERTY()

UICheckBox::UICheckBox(UIView* parent)
 : UIRadioButton(parent)
{

}

UICheckBox::~UICheckBox()
{

}

void UICheckBox::SetChecked(BOOL b)
{
	if(m_bChecked == b)
		return;

	m_bChecked = b;
	Invalidate();

	UIActionEvent event(this, GetID(), ACTION_SELECT, m_bChecked);
	UIApplication::SendEvent(&event);
}

BOOL UICheckBox::OnActionPerformed(UIActionEvent* event)
{
	if(event->GetCommand() == ACTION_CLICK)
	{
		SetChecked(m_bChecked ? FALSE : TRUE);
		return TRUE;
	}
	else
	{
		return OnActionPerformed(event);
	}
}
