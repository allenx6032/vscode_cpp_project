#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Resource.h"

#include <SFML_Font.hpp>

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class Font : public Resource
{
public:

    Font();
    virtual ~Font();

    void        SetSFFont(sf::Font* _pSFFont);
    sf::Font*   GetSFFont() const;
    
    virtual EResourceType::Type GetResourceType() const override;

    virtual bool LoadFromFile() override;

protected:

    virtual void Unload() override;

protected:

    sf::Font* m_sfFont;
};

}   // namespace gugu
