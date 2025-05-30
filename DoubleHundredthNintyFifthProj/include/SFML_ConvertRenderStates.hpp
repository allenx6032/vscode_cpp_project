////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_CONVERTRENDERSTATES_H
#define SFML_CONVERTRENDERSTATES_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML_RenderStates.h>
#include <SFML_RenderStates.hpp>
#include <SFML_ConvertTransform.hpp>
#include <SFML_TextureStruct.h>
#include <SFML_ShaderStruct.h>


////////////////////////////////////////////////////////////
// Convert sfRenderStates* to sf::RenderStates
////////////////////////////////////////////////////////////
inline sf::RenderStates convertRenderStates(const sfRenderStates* states)
{
    sf::RenderStates sfmlStates;

    if (states)
    {
        sfmlStates.blendMode.colorSrcFactor = static_cast<sf::BlendMode::Factor>(states->blendMode.colorSrcFactor);
        sfmlStates.blendMode.colorDstFactor = static_cast<sf::BlendMode::Factor>(states->blendMode.colorDstFactor);
        sfmlStates.blendMode.colorEquation = static_cast<sf::BlendMode::Equation>(states->blendMode.colorEquation);
        sfmlStates.blendMode.alphaSrcFactor = static_cast<sf::BlendMode::Factor>(states->blendMode.alphaSrcFactor);
        sfmlStates.blendMode.alphaDstFactor = static_cast<sf::BlendMode::Factor>(states->blendMode.alphaDstFactor);
        sfmlStates.blendMode.alphaEquation = static_cast<sf::BlendMode::Equation>(states->blendMode.alphaEquation);
        sfmlStates.transform = convertTransform(states->transform);
        sfmlStates.texture = states->texture ? states->texture->This : NULL;
        sfmlStates.shader = states->shader ? &states->shader->This : NULL;
    }

    return sfmlStates;
}


#endif // SFML_CONVERTRENDERSTATES_H
