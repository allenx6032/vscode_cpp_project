////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ElementSFDrawable.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Renderer.h"
#include "Gugu_Memory.h"

#include <SFML_Drawable.hpp>
#include <SFML_RenderTarget.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
ElementSFDrawable::ElementSFDrawable()
    : m_sfDrawable(nullptr)
    , m_callbackOnSizeChanged(nullptr)
{
}

ElementSFDrawable::~ElementSFDrawable()
{
    SafeDelete(m_sfDrawable);
}

void ElementSFDrawable::SetSFDrawable(sf::Drawable* _pSFDrawable)
{
    SafeDelete(m_sfDrawable);

    m_sfDrawable = _pSFDrawable;
}

sf::Drawable* ElementSFDrawable::GetSFDrawable() const
{
    return m_sfDrawable;
}

void ElementSFDrawable::SetCallbackOnSizeChanged(const DelegateElementSizeChanged& callbackOnSizeChanged)
{
    m_callbackOnSizeChanged = callbackOnSizeChanged;
}

void ElementSFDrawable::OnSizeChanged()
{
    if (m_callbackOnSizeChanged)
    {
        m_callbackOnSizeChanged(this);
    }
}

void ElementSFDrawable::RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf)
{
    // TODO: handle culling (need bounds).
    if (m_sfDrawable)
    {
        _kRenderPass.target->draw(*m_sfDrawable, _kTransformSelf);

        //Stats
        if (_kRenderPass.frameInfos)
        {
            _kRenderPass.frameInfos->statDrawCalls += 1;
            // TODO: find  away to retrieve triangles count.
        }

        _kRenderPass.statRenderedDrawables += 1;
    }
}

}   // namespace gugu
