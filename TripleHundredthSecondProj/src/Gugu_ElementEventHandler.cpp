////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ElementEventHandler.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Element.h"
#include "Gugu_Window.h"
#include "Gugu_WindowEventHandler.h"
#include "Gugu_Logger.h"

#include <assert.h>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementEventHandler::ElementEventHandler(Element* element)
    : m_element(element)
    , m_handler(nullptr)
    , m_registeredInteractions(EInteractionType::None)
    , m_disabledInteractions(EInteractionType::None)
    , m_allInteractionsDisabled(false)
    , m_processingInteractionCallbacks(false)
    , m_processingElementCallbacks(false)
{
}

ElementEventHandler::~ElementEventHandler()
{
    assert(!m_processingInteractionCallbacks);  // Destructor is called while callbacks are still being processed.
    assert(!m_processingElementCallbacks);      // Destructor is called while callbacks are still being processed.

    RemoveAllCallbacks();
    UnregisterWindowEventHandler();
}

Element* ElementEventHandler::GetElement() const
{
    return m_element;
}

void ElementEventHandler::UnregisterWindowEventHandler()
{
    if (m_handler)
    {
        // m_handler will be set to null inside this call.
        m_handler->UnregisterElementEventHandler(this);
    }
}

void ElementEventHandler::SetAllInteractionsDisabled(bool disabled)
{
    if (m_allInteractionsDisabled == disabled)
        return;

    m_allInteractionsDisabled = disabled;

    if (m_allInteractionsDisabled)
    {
        FireCallbacks(EElementEvent::InteractionsDisabled);
    }
    else
    {
        FireCallbacks(EElementEvent::InteractionsEnabled);
    }
}

void ElementEventHandler::SetInteractionDisabled(EInteractionType::Type interactionType, bool disabled)
{
    if (disabled)
    {
        m_disabledInteractions = m_disabledInteractions | interactionType;
    }
    else
    {
        m_disabledInteractions = m_disabledInteractions & ~interactionType;
    }
}

bool ElementEventHandler::IsInteractionDisabled(EInteractionType::Type interactionType) const
{
    return (m_disabledInteractions & interactionType) == EInteractionType::None;
}

bool ElementEventHandler::IsInteractionRegisteredAndEnabled(EInteractionType::Type interactionType) const
{
    return !m_allInteractionsDisabled
        && (m_registeredInteractions & interactionType) == interactionType
        && (m_disabledInteractions & interactionType) == EInteractionType::None
        && m_element->IsVisible(true);
}

void ElementEventHandler::AddCallback(EInteractionEvent::Type event, const DelegateInteractionEvent& callback)
{
    if (!callback || event == EInteractionEvent::None)
        return;

    // TODO: I could handle add/remove when processing callbacks, like for Signals.
    assert(!m_processingInteractionCallbacks);  // Removal is called while callbacks are still being processed.

    InteractionCallbackInfos kInfos;
    kInfos.event = event;
    kInfos.callback = callback;
    m_interactionCallbacks.push_back(kInfos);

    CheckRegistration(event);
}

void ElementEventHandler::RemoveCallbacks(EInteractionEvent::Type event)
{
    // TODO: I could handle add/remove when processing callbacks, like for Signals.
    assert(!m_processingInteractionCallbacks);  // Removal is called while callbacks are still being processed.

    // TODO: update registration ? (both filter and registration in the handler)
    for (auto iteCallback = m_interactionCallbacks.begin(); iteCallback != m_interactionCallbacks.end();)
    {
        if (iteCallback->event == event)
        {
            iteCallback = m_interactionCallbacks.erase(iteCallback);
        }
        else
        {
             ++iteCallback;
        }
    }
}

void ElementEventHandler::FireCallbacks(EInteractionEvent::Type event, const InteractionInfos& interactionInfos)
{
    m_processingInteractionCallbacks = true;

    for (size_t i = 0; i < m_interactionCallbacks.size(); ++i)
    {
        if (m_interactionCallbacks[i].event == event)
        {
            m_interactionCallbacks[i].callback(interactionInfos);

            if (interactionInfos.absorbEvent)
                break;
        }
    }

    m_processingInteractionCallbacks = false;
}

void ElementEventHandler::AddCallback(EElementEvent::Type event, const Handle& handle, const Callback& callback)
{
    if (!callback || event == EElementEvent::None)
        return;

    // TODO: I could handle add/remove when processing callbacks, like for Signals.
    assert(!m_processingElementCallbacks);  // Removal is called while callbacks are still being processed.

    ElementCallbackInfos kInfos;
    kInfos.event = event;
    kInfos.callback = callback;
    kInfos.handle = handle;
    m_elementCallbacks.push_back(kInfos);
}

void ElementEventHandler::RemoveCallbacks(EElementEvent::Type event, const Handle& handle)
{
    // TODO: I could handle add/remove when processing callbacks, like for Signals.
    assert(!m_processingElementCallbacks);  // Removal is called while callbacks are still being processed.

    for (auto iteCallback = m_elementCallbacks.begin(); iteCallback != m_elementCallbacks.end();)
    {
        if (iteCallback->event == event && iteCallback->handle == handle)
        {
            iteCallback = m_elementCallbacks.erase(iteCallback);
        }
        else
        {
            ++iteCallback;
        }
    }
}

void ElementEventHandler::FireCallbacks(EElementEvent::Type event)
{
    m_processingElementCallbacks = true;

    for (size_t i = 0; i < m_elementCallbacks.size(); ++i)
    {
        if (m_elementCallbacks[i].event == event)
        {
            m_elementCallbacks[i].callback();
        }
    }

    m_processingElementCallbacks = false;
}

void ElementEventHandler::RemoveAllCallbacks()
{
    m_registeredInteractions = 0;
    m_interactionCallbacks.clear();
    m_elementCallbacks.clear();
}

void ElementEventHandler::CheckRegistration(EInteractionEvent::Type event)
{
    EInteractionType::Type interactionType = EInteractionType::None;

    switch (event)
    {
    case EInteractionEvent::MouseEntered:
    case EInteractionEvent::MouseLeft:
        interactionType = EInteractionType::Focus;
        break;
    case EInteractionEvent::MousePressed:
    case EInteractionEvent::MouseReleased:
        interactionType = EInteractionType::Click;
        break;
    case EInteractionEvent::MouseSelected:
    case EInteractionEvent::MouseDeselected:
        interactionType = EInteractionType::Selection;
        break;
    case EInteractionEvent::MouseScrolled:
        interactionType = EInteractionType::Scroll;
        break;
    case EInteractionEvent::MouseDragBegan:
    case EInteractionEvent::MouseDragMoved:
    case EInteractionEvent::MouseDragEnded:
        interactionType = EInteractionType::Drag;
        break;
    case EInteractionEvent::RawSFEvent:
        interactionType = EInteractionType::RawSFEvent;
        break;
    default:
        break;
    }

    if (interactionType != EInteractionType::None)
    {
        m_registeredInteractions = m_registeredInteractions | interactionType;
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, interactionType);
    }
}

}   // namespace gugu
