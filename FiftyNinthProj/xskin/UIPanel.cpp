#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "UIPanel.h"
#include "UIResponser.h"
#include "UIRootView.h"
#include "UIWheelEvent.h"
#include "UIKeyEvent.h"

UIPanel::UIPanel(UIResponser* owner, UIWindow* parent)
 : UIWindow(parent)
{
	m_pOwner = owner;
}

UIPanel::~UIPanel()
{

}

void UIPanel::ForwardWheelEvent(UIWheelEvent* event)
{
	m_pRootView->ProcessMouseWheel(event->GetLocation(), event->GetDelta(), event->GetFlags());
}

void UIPanel::ForwardKeyPressedEvent(UIKeyEvent* event)
{
	m_pRootView->ProcessKeyPressed(event->GetKeyCode(), event->GetRepeatCount(), event->GetFlags());
}

void UIPanel::ForwardKeyReleasedEvent(UIKeyEvent* event)
{
	m_pRootView->ProcessKeyReleased(event->GetKeyCode(), event->GetRepeatCount(), event->GetFlags());
}

DWORD UIPanel::GetWndStyle() const
{
	return WS_POPUP;
}

DWORD UIPanel::GetWndExStyle() const
{
	return WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
}

UIResponser* UIPanel::GetNextResponser() const
{
	return m_pOwner;
}

LRESULT UIPanel::OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_MOUSEACTIVATE)
	{
		return MA_NOACTIVATE;
	}
	return OnWindowMessage(uMsg, wParam, lParam);
}