#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ElementText.h"
#include "Gugu_Callback.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace sf
{
    class Event;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class ElementEditableText : public ElementText
{
public:

    ElementEditableText();
    virtual ~ElementEditableText();

    void SetEditable    (bool _bIsEditable);

    void StartEdition   ();
    void StopEdition    ();

    void ProcessSFEvent(const sf::Event& _oSFEvent);

    void SetOnValidate(const Callback& callbackOnValidate);

protected:

    void OnSFEvent(const InteractionInfos& interactionInfos);

    void OnMouseSelected();
    void OnMouseDeselected();

    void StartEditionImpl();
    void StopEditionImpl();

    virtual void RecomputeImpl() override;
    virtual void OnTextRendered(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf) override;

protected:

    sf::Text* m_sfTextCursor;

    bool m_isEditable;
    bool m_isEditing;
    bool m_isTickDisplayed;
    float m_timeSinceTick;

    Callback m_callbackOnValidate;
};

}   // namespace gugu
