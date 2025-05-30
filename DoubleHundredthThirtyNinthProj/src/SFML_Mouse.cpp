////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML_Mouse.hpp>
#include <SFML_InputImpl.hpp>
#include <SFML_Window_Window.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
bool Mouse::isButtonPressed(Button button)
{
    return priv::InputImpl::isMouseButtonPressed(button);
}


////////////////////////////////////////////////////////////
Vector2i Mouse::getPosition()
{
    return priv::InputImpl::getMousePosition();
}


////////////////////////////////////////////////////////////
Vector2i Mouse::getPosition(const Window& relativeTo)
{
    return priv::InputImpl::getMousePosition(relativeTo);
}


////////////////////////////////////////////////////////////
void Mouse::setPosition(const Vector2i& position)
{
    priv::InputImpl::setMousePosition(position);
}


////////////////////////////////////////////////////////////
void Mouse::setPosition(const Vector2i& position, const Window& relativeTo)
{
    priv::InputImpl::setMousePosition(position, relativeTo);
}

} // namespace sf
