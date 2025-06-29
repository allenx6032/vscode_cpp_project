////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_Font.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Memory.h"
#include "Gugu_String.h"
#include "Gugu_Logger.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
Font::Font()
: m_sfFont(nullptr)
{
}

Font::~Font()
{
    Unload();
}

void Font::SetSFFont(sf::Font* _pSFFont)
{
    m_sfFont = _pSFFont;
}

sf::Font* Font::GetSFFont() const
{
    return m_sfFont;
}

EResourceType::Type Font::GetResourceType() const
{
    return EResourceType::Font;
}

void Font::Unload()
{
    SafeDelete(m_sfFont);
}

bool Font::LoadFromFile()
{
    Unload();

    m_sfFont = new sf::Font;
    if (!m_sfFont->loadFromFile(GetFileInfo().GetFileSystemPath()))
    {
        GetLogEngine()->Print(ELog::Warning, ELogEngine::Resources, StringFormat("Font not found : {0}", GetFileInfo().GetFilePath_utf8()));
        return false;
    }

    return true;
}

}   // namespace gugu
