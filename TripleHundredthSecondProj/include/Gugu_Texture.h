#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Resource.h"
#include "Gugu_Vector2.h"

#include <SFML_Rect.hpp>

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace sf
{
    class Texture;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class Texture : public Resource
{
public:

    Texture();
    virtual ~Texture();

    void SetSFTexture(sf::Texture* _pSFTexture);
    sf::Texture* GetSFTexture() const;
    
    void SetSmooth(bool smooth);
    bool IsSmooth() const;

    void SetRepeated(bool repeated);
    bool IsRepeated() const;

    Vector2u GetSize() const;
    sf::IntRect GetRect() const;

    virtual EResourceType::Type GetResourceType() const override;

    virtual bool LoadFromFile() override;

protected:

    virtual void Unload() override;

protected:

    sf::Texture* m_sfTexture;
};

}   // namespace gugu
