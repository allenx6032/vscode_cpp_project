//-------------------------------------------------------------------------------------------------
// file:    InputSystem.cpp
// author:  Andy Ellinger
// brief:   Input system
//
// Copyright � 2022 DigiPen, All rights reserved.
//-------------------------------------------------------------------------------------------------

// module;

#include "DGL.h"

// module Input;

// import Errors;
// import WindowsSystem;
#include "DGL_InputSystem.ixx"
#include "DGL_Error.ixx"
#include "DGL_WindowsSystem.ixx"

namespace DGL
{

InputSystem* gInput = nullptr;

//------------------------------------------------------------------------------------- InputSystem

//*************************************************************************************************
InputSystem::InputSystem()
{
    gInput = this;
}

//*************************************************************************************************
void InputSystem::Update()
{
    // Copy the current key state to the previous key state array.
    mPrevKeyState = mKeyState;

    // We don't need to reset the current key state array: we want it to save its previous values.
    // A key is considered down until the WM_KEYUP message is received.

    // However, if the window loses focus, we do want to reset, since we don't get messages about
    // any keys that are released while we aren't the current window
    if (!gWinSys->GetWindowHasFocus())
        mKeyState.fill(false);

    // Use the Windows point struct to get the cursor position.
    POINT mouseCoord;
    GetCursorPos(&mouseCoord);
    // Adjust the position to be relative to our window instead of the whole screen.
    ScreenToClient(gWinSys->GetWindowHandle(), &mouseCoord);

    // Save the change in mouse position using our variable which has the value from last frame.
    mMouseDelta.x = mouseCoord.x - mMousePos.x;
    mMouseDelta.y = mouseCoord.y - mMousePos.y;

    // Save the current mouse position into our variable.
    mMousePos = { (float)mouseCoord.x, (float)mouseCoord.y };

    // Reset the mouse wheel delta variable. 
    mMouseWheelDelta = 0;

    // Reset the last key triggered variable.
    mLastKeyTriggered = 0;
}

//*************************************************************************************************
int InputSystem::GetMouseScrollDelta() const
{
    return mMouseWheelDelta;
}

//*************************************************************************************************
DGL_Vec2 InputSystem::GetMousePosition() const
{
    return mMousePos;
}

//*************************************************************************************************
DGL_Vec2 InputSystem::GetMousePositionDelta() const
{
    return mMouseDelta;
}

//*************************************************************************************************
void InputSystem::SetKeyState(unsigned char key, bool state)
{
    if (key >= cInputKeyCount || key < 0)
    {
        gError->SetError("Invalid character key passed into SetKeyState");
        return;
    }

    mKeyState[key] = state;

    // If setting this key to down and it was previously up, store as last key triggered
    if (state && !mPrevKeyState[key])
        mLastKeyTriggered = key;
}

//*************************************************************************************************
void InputSystem::SetMouseScrollDelta(int delta)
{
    mMouseWheelDelta = delta;
}

//*************************************************************************************************
bool InputSystem::KeyDown(unsigned char key) const
{
    if (key >= cInputKeyCount || key < 0)
    {
        gError->SetError("Invalid character key passed into KeyDown");
        return false;
    }

    // If the key state is true, the key is down.
    return mKeyState[key] == true;
}

//*************************************************************************************************
bool InputSystem::KeyTriggered(unsigned char key) const
{
    if (key >= cInputKeyCount || key < 0)
    {
        gError->SetError("Invalid character key passed into KeyTriggered");
        return false;
    }

    // If the key state is currently true but it was false previously, then it has been triggered.
    return mKeyState[key] == true && mPrevKeyState[key] == false;
}

//*************************************************************************************************
bool InputSystem::KeyReleased(unsigned char key) const
{
    if (key >= cInputKeyCount || key < 0)
    {
        gError->SetError("Invalid character key passed into KeyReleased");
        return false;
    }

    // If the key state is currently false but it was true previously, then it has been released.
    return mKeyState[key] == false && mPrevKeyState[key] == true;
}

//*************************************************************************************************
unsigned char InputSystem::LastKeyTriggered() const
{
    return mLastKeyTriggered;
}

} // namespace DGL

using namespace DGL;

//*************************************************************************************************
int DGL_Input_GetMouseScrollDelta(void)
{
    return gInput->GetMouseScrollDelta();
}

//*************************************************************************************************
DGL_Vec2 DGL_Input_GetMousePosition(void)
{
    return gInput->GetMousePosition();
}

//*************************************************************************************************
DGL_Vec2 DGL_Input_GetMousePositionDelta(void)
{
    return gInput->GetMousePositionDelta();
}

//*************************************************************************************************
BOOL DGL_Input_KeyDown(unsigned char key)
{
    return gInput->KeyDown(key);
}

//*************************************************************************************************
BOOL DGL_Input_KeyTriggered(unsigned char key)
{
    return gInput->KeyTriggered(key);
}

//*************************************************************************************************
BOOL DGL_Input_KeyReleased(unsigned char key)
{
    return gInput->KeyReleased(key);
}

//*************************************************************************************************
unsigned char DGL_Input_LastKeyTriggered(void)
{
    return gInput->LastKeyTriggered();
}

//*************************************************************************************************
void DGL_Input_ShowCursor(BOOL show)
{
    if (show)
    {
        int count = ShowCursor(true);
        while (count <= 0)
            count = ShowCursor(true);
    }
    else
    {
        int count = ShowCursor(false);
        while (count > -1)
            count = ShowCursor(false);
    }
}


