////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_EventListener.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_WindowEventHandler.h"
#include "Gugu_Window.h"

#include <SFML_Event.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
EventListener::EventListener()
{
    m_handler = nullptr;
}

EventListener::~EventListener()
{
    UnregisterEventHandler();
}

void EventListener::RegisterEventHandler(Window* _pWindow)
{
    if (_pWindow && _pWindow->GetEventHandler())
    {
        RegisterEventHandler(_pWindow->GetEventHandler());
    }
}

void EventListener::RegisterEventHandler(WindowEventHandler* _pHandler)
{
    if (_pHandler)
    {
        _pHandler->AddEventListener(this);
    }
}

void EventListener::UnregisterEventHandler()
{
    if (m_handler)
    {
        m_handler->RemoveEventListener(this);
    }
}

bool EventListener::IsRegistered() const
{
    return (m_handler != nullptr);
}

bool EventListener::IsRegistered(Window* _pWindow) const
{
    return (m_handler != nullptr && _pWindow != nullptr && m_handler == _pWindow->GetEventHandler());
}

bool EventListener::IsRegistered(WindowEventHandler* _pHandler) const
{
    return (m_handler != nullptr && m_handler == _pHandler);
}

bool EventListener::OnSFEvent(const sf::Event& _oSFEvent)
{
    return true;
}

}   // namespace gugu
