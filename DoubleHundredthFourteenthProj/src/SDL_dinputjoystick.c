/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#include "SDL_sysjoystick.h"

#if SDL_JOYSTICK_DINPUT

#include "SDL_windowsjoystick_c.h"
#include "SDL_dinputjoystick_c.h"
#include "SDL_rawinputjoystick_c.h"
#include "SDL_xinputjoystick_c.h"
#include "SDL_hidapijoystick_c.h"

#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL      0x80000000
#endif

#define INPUT_QSIZE 32      /* Buffer up to 32 input messages */
#define JOY_AXIS_THRESHOLD  (((SDL_JOYSTICK_AXIS_MAX)-(SDL_JOYSTICK_AXIS_MIN))/100)   /* 1% motion */

#define CONVERT_MAGNITUDE(x)    (((x)*10000) / 0x7FFF)

/* external variables referenced. */
extern HWND SDL_HelperWindow;

/* local variables */
static SDL_bool coinitialized = SDL_FALSE;
static LPDIRECTINPUT8 dinput = NULL;
static PRAWINPUTDEVICELIST SDL_RawDevList = NULL;
static UINT SDL_RawDevListCount = 0;

/* Taken from Wine - Thanks! */
static DIOBJECTDATAFORMAT dfDIJoystick2[] = {
    { &GUID_XAxis, DIJOFS_X, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, DIJOFS_Y, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_ZAxis, DIJOFS_Z, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RxAxis, DIJOFS_RX, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RyAxis, DIJOFS_RY, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RzAxis, DIJOFS_RZ, DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, DIJOFS_SLIDER(0), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, DIJOFS_SLIDER(1), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_POV, DIJOFS_POV(0), DIDFT_OPTIONAL | DIDFT_POV | DIDFT_ANYINSTANCE, 0 },
    { &GUID_POV, DIJOFS_POV(1), DIDFT_OPTIONAL | DIDFT_POV | DIDFT_ANYINSTANCE, 0 },
    { &GUID_POV, DIJOFS_POV(2), DIDFT_OPTIONAL | DIDFT_POV | DIDFT_ANYINSTANCE, 0 },
    { &GUID_POV, DIJOFS_POV(3), DIDFT_OPTIONAL | DIDFT_POV | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(0), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(1), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(2), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(3), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(4), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(5), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(6), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(7), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(8), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(9), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(10), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(11), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(12), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(13), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(14), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(15), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(16), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(17), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(18), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(19), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(20), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(21), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(22), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(23), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(24), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(25), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(26), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(27), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(28), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(29), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(30), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(31), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(32), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(33), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(34), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(35), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(36), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(37), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(38), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(39), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(40), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(41), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(42), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(43), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(44), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(45), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(46), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(47), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(48), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(49), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(50), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(51), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(52), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(53), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(54), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(55), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(56), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(57), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(58), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(59), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(60), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(61), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(62), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(63), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(64), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(65), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(66), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(67), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(68), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(69), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(70), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(71), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(72), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(73), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(74), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(75), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(76), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(77), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(78), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(79), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(80), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(81), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(82), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(83), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(84), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(85), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(86), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(87), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(88), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(89), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(90), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(91), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(92), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(93), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(94), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(95), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(96), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(97), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(98), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(99), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(100), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(101), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(102), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(103), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(104), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(105), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(106), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(107), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(108), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(109), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(110), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(111), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(112), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(113), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(114), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(115), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(116), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(117), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(118), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(119), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(120), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(121), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(122), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(123), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(124), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(125), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(126), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { NULL, DIJOFS_BUTTON(127), DIDFT_OPTIONAL | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { &GUID_XAxis, FIELD_OFFSET(DIJOYSTATE2, lVX), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, FIELD_OFFSET(DIJOYSTATE2, lVY), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_ZAxis, FIELD_OFFSET(DIJOYSTATE2, lVZ), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RxAxis, FIELD_OFFSET(DIJOYSTATE2, lVRx), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RyAxis, FIELD_OFFSET(DIJOYSTATE2, lVRy), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RzAxis, FIELD_OFFSET(DIJOYSTATE2, lVRz), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglVSlider[0]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglVSlider[1]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_XAxis, FIELD_OFFSET(DIJOYSTATE2, lAX), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, FIELD_OFFSET(DIJOYSTATE2, lAY), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_ZAxis, FIELD_OFFSET(DIJOYSTATE2, lAZ), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RxAxis, FIELD_OFFSET(DIJOYSTATE2, lARx), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RyAxis, FIELD_OFFSET(DIJOYSTATE2, lARy), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RzAxis, FIELD_OFFSET(DIJOYSTATE2, lARz), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglASlider[0]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglASlider[1]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_XAxis, FIELD_OFFSET(DIJOYSTATE2, lFX), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, FIELD_OFFSET(DIJOYSTATE2, lFY), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_ZAxis, FIELD_OFFSET(DIJOYSTATE2, lFZ), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RxAxis, FIELD_OFFSET(DIJOYSTATE2, lFRx), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RyAxis, FIELD_OFFSET(DIJOYSTATE2, lFRy), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_RzAxis, FIELD_OFFSET(DIJOYSTATE2, lFRz), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglFSlider[0]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Slider, FIELD_OFFSET(DIJOYSTATE2, rglFSlider[1]), DIDFT_OPTIONAL | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
};

const DIDATAFORMAT SDL_c_dfDIJoystick2 = {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDF_ABSAXIS,
    sizeof(DIJOYSTATE2),
    SDL_arraysize(dfDIJoystick2),
    dfDIJoystick2
};

/* Convert a DirectInput return code to a text message */
static int
SetDIerror(const char *function, HRESULT code)
{
    return SDL_SetError("%s() DirectX error 0x%8.8lx", function, code);
}

#if 0 /* Microsoft recommended implementation, but slower than checking raw devices */
#define COBJMACROS
#include <wbemidl.h>
#include <oleauto.h>

static const IID CLSID_WbemLocator = { 0x4590f811, 0x1d3a, 0x11d0,{ 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 } };
static const IID IID_IWbemLocator = { 0xdc12a687, 0x737f, 0x11cf,{ 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 } };

static SDL_bool
WIN_IsXInputDevice(const WCHAR *name, const GUID* pGuidProductFromDirectInput)
{
    IWbemLocator*           pIWbemLocator = NULL;
    IEnumWbemClassObject*   pEnumDevices = NULL;
    IWbemClassObject*       pDevices[20];
    IWbemServices*          pIWbemServices = NULL;
    BSTR                    bstrNamespace = NULL;
    BSTR                    bstrDeviceID = NULL;
    BSTR                    bstrClassName = NULL;
    DWORD                   uReturned = 0;
    SDL_bool                bIsXinputDevice = SDL_FALSE;
    UINT                    iDevice = 0;
    VARIANT                 var;
    HRESULT                 hr;

    if (!SDL_XINPUT_Enabled()) {
        return SDL_FALSE;
    }

    if (SDL_wcsstr(name, L" XINPUT ") != NULL) {
        /* This is a duplicate interface for a controller that will show up with XInput,
           e.g. Xbox One Elite Series 2 in Bluetooth mode.
         */
        return SDL_TRUE;
    }

    SDL_zeroa(pDevices);

    // Create WMI
    hr = CoCreateInstance(&CLSID_WbemLocator,
        NULL,
        CLSCTX_INPROC_SERVER,
        &IID_IWbemLocator,
        (LPVOID*)&pIWbemLocator);
    if (FAILED(hr) || pIWbemLocator == NULL)
        goto LCleanup;

    bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
    bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
    bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

    // Connect to WMI 
    hr = IWbemLocator_ConnectServer(pIWbemLocator, bstrNamespace, NULL, NULL, 0L,
        0L, NULL, NULL, &pIWbemServices);
    if (FAILED(hr) || pIWbemServices == NULL) {
        goto LCleanup;
    }

    // Switch security level to IMPERSONATE. 
    CoSetProxyBlanket((IUnknown *)pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    hr = IWbemServices_CreateInstanceEnum(pIWbemServices, bstrClassName, 0, NULL, &pEnumDevices);
    if (FAILED(hr) || pEnumDevices == NULL)
        goto LCleanup;

    // Loop over all devices
    for (;;) {
        // Get 20 at a time
        hr = IEnumWbemClassObject_Next(pEnumDevices, 10000, SDL_arraysize(pDevices), pDevices, &uReturned);
        if (FAILED(hr)) {
            goto LCleanup;
        }
        if (uReturned == 0) {
            break;
        }

        for (iDevice = 0; iDevice < uReturned; iDevice++) {
            // For each device, get its device ID
            hr = IWbemClassObject_Get(pDevices[iDevice], bstrDeviceID, 0L, &var, NULL, NULL);
            if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL) {
                // Check if the device ID contains "IG_".  If it does, then it's an XInput device
                // This information can not be found from DirectInput 
                if (SDL_wcsstr(var.bstrVal, L"IG_")) {
                    char *bstrVal = WIN_StringToUTF8(var.bstrVal);

                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0, dwVidPid;
                    const char *strVid, *strPid;
                    strVid = SDL_strstr(bstrVal, "VID_");
                    if (strVid && SDL_sscanf(strVid, "VID_%4X", &dwVid) != 1)
                        dwVid = 0;
                    strPid = SDL_strstr(bstrVal, "PID_");
                    if (strPid && SDL_sscanf(strPid, "PID_%4X", &dwPid) != 1)
                        dwPid = 0;

                    SDL_free(bstrVal);

                    // Compare the VID/PID to the DInput device
                    dwVidPid = MAKELONG(dwVid, dwPid);
                    if (dwVidPid == pGuidProductFromDirectInput->Data1) {
                        bIsXinputDevice = SDL_TRUE;
                        goto LCleanup;
                    }
                }
            }
            IWbemClassObject_Release(pDevices[iDevice]);
        }
    }

LCleanup:
    if (bstrNamespace) {
        SysFreeString(bstrNamespace);
    }
    if (bstrDeviceID) {
        SysFreeString(bstrDeviceID);
    }
    if (bstrClassName) {
        SysFreeString(bstrClassName);
    }
    for (iDevice = 0; iDevice < SDL_arraysize(pDevices); iDevice++) {
        if (pDevices[iDevice]) {
            IWbemClassObject_Release(pDevices[iDevice]);
        }
    }
    if (pEnumDevices) {
        IEnumWbemClassObject_Release(pEnumDevices);
    }
    if (pIWbemLocator) {
        IWbemLocator_Release(pIWbemLocator);
    }
    if (pIWbemServices) {
        IWbemServices_Release(pIWbemServices);
    }

    return bIsXinputDevice;
}
#endif /* 0 */

static SDL_bool
SDL_IsXInputDevice(const WCHAR *name, const GUID* pGuidProductFromDirectInput)
{
    UINT i;

    if (!SDL_XINPUT_Enabled()) {
        return SDL_FALSE;
    }

    if (SDL_wcsstr(name, L" XINPUT ") != NULL) {
        /* This is a duplicate interface for a controller that will show up with XInput,
           e.g. Xbox One Elite Series 2 in Bluetooth mode.
         */
        return SDL_TRUE;
    }

    if (SDL_memcmp(&pGuidProductFromDirectInput->Data4[2], "PIDVID", 6) == 0) {
        Uint16 vendor_id = (Uint16)LOWORD(pGuidProductFromDirectInput->Data1);
        Uint16 product_id = (Uint16)HIWORD(pGuidProductFromDirectInput->Data1);
        SDL_GameControllerType type = SDL_GetJoystickGameControllerType("", vendor_id, product_id, -1, 0, 0, 0);
        if (type == SDL_CONTROLLER_TYPE_XBOX360 ||
            type == SDL_CONTROLLER_TYPE_XBOXONE ||
            (vendor_id == 0x28DE && product_id == 0x11FF)) {
            return SDL_TRUE;
        }
    }

    /* Go through RAWINPUT (WinXP and later) to find HID devices. */
    /* Cache this if we end up using it. */
    if (SDL_RawDevList == NULL) {
        if ((GetRawInputDeviceList(NULL, &SDL_RawDevListCount, sizeof(RAWINPUTDEVICELIST)) == -1) || (!SDL_RawDevListCount)) {
            return SDL_FALSE;  /* oh well. */
        }

        SDL_RawDevList = (PRAWINPUTDEVICELIST)SDL_malloc(sizeof(RAWINPUTDEVICELIST) * SDL_RawDevListCount);
        if (SDL_RawDevList == NULL) {
            SDL_OutOfMemory();
            return SDL_FALSE;
        }

        if (GetRawInputDeviceList(SDL_RawDevList, &SDL_RawDevListCount, sizeof(RAWINPUTDEVICELIST)) == -1) {
            SDL_free(SDL_RawDevList);
            SDL_RawDevList = NULL;
            return SDL_FALSE;  /* oh well. */
        }
    }

    for (i = 0; i < SDL_RawDevListCount; i++) {
        RID_DEVICE_INFO rdi;
        char devName[MAX_PATH];
        UINT rdiSize = sizeof(rdi);
        UINT nameSize = SDL_arraysize(devName);

        rdi.cbSize = sizeof(rdi);
        if ((SDL_RawDevList[i].dwType == RIM_TYPEHID) &&
            (GetRawInputDeviceInfoA(SDL_RawDevList[i].hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize) != ((UINT)-1)) &&
            (MAKELONG(rdi.hid.dwVendorId, rdi.hid.dwProductId) == ((LONG)pGuidProductFromDirectInput->Data1)) &&
            (GetRawInputDeviceInfoA(SDL_RawDevList[i].hDevice, RIDI_DEVICENAME, devName, &nameSize) != ((UINT)-1)) &&
            (SDL_strstr(devName, "IG_") != NULL)) {
            return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}

void FreeRumbleEffectData(DIEFFECT *effect)
{
    if (!effect) {
        return;
    }
    SDL_free(effect->rgdwAxes);
    SDL_free(effect->rglDirection);
    SDL_free(effect->lpvTypeSpecificParams);
    SDL_free(effect);
}

DIEFFECT *CreateRumbleEffectData(Sint16 magnitude)
{
    DIEFFECT *effect;
    DIPERIODIC *periodic;

    /* Create the effect */
    effect = (DIEFFECT *)SDL_calloc(1, sizeof(*effect));
    if (!effect) {
        return NULL;
    }
    effect->dwSize = sizeof(*effect);
    effect->dwGain = 10000;
    effect->dwFlags = DIEFF_OBJECTOFFSETS;
    effect->dwDuration = SDL_MAX_RUMBLE_DURATION_MS * 1000; /* In microseconds. */
    effect->dwTriggerButton = DIEB_NOTRIGGER;

    effect->cAxes = 2;
    effect->rgdwAxes = (DWORD *)SDL_calloc(effect->cAxes, sizeof(DWORD));
    if (!effect->rgdwAxes) {
        FreeRumbleEffectData(effect);
        return NULL;
    }

    effect->rglDirection = (LONG *)SDL_calloc(effect->cAxes, sizeof(LONG));
    if (!effect->rglDirection) {
        FreeRumbleEffectData(effect);
        return NULL;
    }
    effect->dwFlags |= DIEFF_CARTESIAN;

    periodic = (DIPERIODIC *)SDL_calloc(1, sizeof(*periodic));
    if (!periodic) {
        FreeRumbleEffectData(effect);
        return NULL;
    }
    periodic->dwMagnitude = CONVERT_MAGNITUDE(magnitude);
    periodic->dwPeriod = 1000000;

    effect->cbTypeSpecificParams = sizeof(*periodic);
    effect->lpvTypeSpecificParams = periodic;

    return effect;
}

int
SDL_DINPUT_JoystickInit(void)
{
    HRESULT result;
    HINSTANCE instance;

    result = WIN_CoInitialize();
    if (FAILED(result)) {
        return SetDIerror("CoInitialize", result);
    }

    coinitialized = SDL_TRUE;

    result = CoCreateInstance(&CLSID_DirectInput8, NULL, CLSCTX_INPROC_SERVER,
        &IID_IDirectInput8, (LPVOID *)&dinput);

    if (FAILED(result)) {
        return SetDIerror("CoCreateInstance", result);
    }

    /* Because we used CoCreateInstance, we need to Initialize it, first. */
    instance = GetModuleHandle(NULL);
    if (instance == NULL) {
        IDirectInput8_Release(dinput);
        dinput = NULL;
        return SDL_SetError("GetModuleHandle() failed with error code %lu.", GetLastError());
    }
    result = IDirectInput8_Initialize(dinput, instance, DIRECTINPUT_VERSION);

    if (FAILED(result)) {
        IDirectInput8_Release(dinput);
        dinput = NULL;
        return SetDIerror("IDirectInput::Initialize", result);
    }
    return 0;
}

/* helper function for direct input, gets called for each connected joystick */
static BOOL CALLBACK
EnumJoysticksCallback(const DIDEVICEINSTANCE * pdidInstance, VOID * pContext)
{
    JoyStick_DeviceData *pNewJoystick;
    JoyStick_DeviceData *pPrevJoystick = NULL;
    const DWORD devtype = (pdidInstance->dwDevType & 0xFF);
    Uint16 *guid16;
    Uint16 vendor = 0;
    Uint16 product = 0;
    Uint16 version = 0;
    WCHAR hidPath[MAX_PATH];
    char *name;

    if (devtype == DI8DEVTYPE_SUPPLEMENTAL) {
        /* Add any supplemental devices that should be ignored here */
#define MAKE_TABLE_ENTRY(VID, PID)    ((((DWORD)PID)<<16)|VID)
        static DWORD ignored_devices[] = {
            MAKE_TABLE_ENTRY(0, 0)
        };
#undef MAKE_TABLE_ENTRY
        unsigned int i;

        for (i = 0; i < SDL_arraysize(ignored_devices); ++i) {
            if (pdidInstance->guidProduct.Data1 == ignored_devices[i]) {
                return DIENUM_CONTINUE;
            }
        }
    }

    if (SDL_IsXInputDevice(pdidInstance->tszProductName, &pdidInstance->guidProduct)) {
        return DIENUM_CONTINUE;  /* ignore XInput devices here, keep going. */
    }

    {
        HRESULT result;
        LPDIRECTINPUTDEVICE8 device;
        LPDIRECTINPUTDEVICE8 InputDevice;
        DIPROPGUIDANDPATH dipdw2;

        result = IDirectInput8_CreateDevice(dinput, &(pdidInstance->guidInstance), &device, NULL);
        if (FAILED(result)) {
            return DIENUM_CONTINUE; /* better luck next time? */
        }

        /* Now get the IDirectInputDevice8 interface, instead. */
        result = IDirectInputDevice8_QueryInterface(device, &IID_IDirectInputDevice8, (LPVOID *)&InputDevice);
        /* We are done with this object.  Use the stored one from now on. */
        IDirectInputDevice8_Release(device);
        if (FAILED(result)) {
            return DIENUM_CONTINUE; /* better luck next time? */
        }
        dipdw2.diph.dwSize = sizeof(dipdw2);
        dipdw2.diph.dwHeaderSize = sizeof(dipdw2.diph);
        dipdw2.diph.dwObj = 0; // device property
        dipdw2.diph.dwHow = DIPH_DEVICE;

        result = IDirectInputDevice8_GetProperty(InputDevice, DIPROP_GUIDANDPATH, &dipdw2.diph);
        IDirectInputDevice8_Release(InputDevice);
        if (FAILED(result)) {
            return DIENUM_CONTINUE; /* better luck next time? */
        }

        /* Get device path, compare that instead of GUID, additionally update GUIDs of joysticks with matching paths, in case they're not open yet. */
        SDL_wcslcpy(hidPath, dipdw2.wszPath, SDL_arraysize(hidPath));
    }

    pNewJoystick = *(JoyStick_DeviceData **)pContext;
    while (pNewJoystick) {
        if (SDL_wcscmp(pNewJoystick->hidPath, hidPath) == 0) {
            /* if we are replacing the front of the list then update it */
            if (pNewJoystick == *(JoyStick_DeviceData **)pContext) {
                *(JoyStick_DeviceData **)pContext = pNewJoystick->pNext;
            } else if (pPrevJoystick) {
                pPrevJoystick->pNext = pNewJoystick->pNext;
            }

            /* Update with new guid/etc, if it has changed */
            SDL_memcpy(&pNewJoystick->dxdevice, pdidInstance, sizeof(DIDEVICEINSTANCE));

            pNewJoystick->pNext = SYS_Joystick;
            SYS_Joystick = pNewJoystick;

            return DIENUM_CONTINUE; /* already have this joystick loaded, just keep going */
        }

        pPrevJoystick = pNewJoystick;
        pNewJoystick = pNewJoystick->pNext;
    }

    pNewJoystick = (JoyStick_DeviceData *)SDL_malloc(sizeof(JoyStick_DeviceData));
    if (!pNewJoystick) {
        return DIENUM_CONTINUE; /* better luck next time? */
    }

    SDL_zerop(pNewJoystick);
    SDL_wcslcpy(pNewJoystick->hidPath, hidPath, SDL_arraysize(pNewJoystick->hidPath));
    SDL_memcpy(&pNewJoystick->dxdevice, pdidInstance, sizeof(DIDEVICEINSTANCE));
    SDL_memset(pNewJoystick->guid.data, 0, sizeof(pNewJoystick->guid.data));

    if (SDL_memcmp(&pdidInstance->guidProduct.Data4[2], "PIDVID", 6) == 0) {
        vendor = (Uint16)LOWORD(pdidInstance->guidProduct.Data1);
        product = (Uint16)HIWORD(pdidInstance->guidProduct.Data1);
    }

    name = WIN_StringToUTF8(pdidInstance->tszProductName);
    pNewJoystick->joystickname = SDL_CreateJoystickName(vendor, product, NULL, name);
    SDL_free(name);

    if (!pNewJoystick->joystickname) {
        SDL_free(pNewJoystick);
        return DIENUM_CONTINUE; /* better luck next time? */
    }

    guid16 = (Uint16 *)pNewJoystick->guid.data;
    if (SDL_memcmp(&pdidInstance->guidProduct.Data4[2], "PIDVID", 6) == 0) {
        *guid16++ = SDL_SwapLE16(SDL_HARDWARE_BUS_USB);
        *guid16++ = 0;
        *guid16++ = SDL_SwapLE16(vendor);
        *guid16++ = 0;
        *guid16++ = SDL_SwapLE16(product);
        *guid16++ = 0;
        *guid16++ = SDL_SwapLE16(version);
        *guid16++ = 0;
    } else {
        *guid16++ = SDL_SwapLE16(SDL_HARDWARE_BUS_BLUETOOTH);
        *guid16++ = 0;
        SDL_strlcpy((char*)guid16, pNewJoystick->joystickname, sizeof(pNewJoystick->guid.data) - 4);
    }

    if (SDL_ShouldIgnoreJoystick(pNewJoystick->joystickname, pNewJoystick->guid)) {
        SDL_free(pNewJoystick->joystickname);
        SDL_free(pNewJoystick);
        return DIENUM_CONTINUE;
    }

#ifdef SDL_JOYSTICK_HIDAPI
    if (HIDAPI_IsDevicePresent(vendor, product, 0, pNewJoystick->joystickname)) {
        /* The HIDAPI driver is taking care of this device */
        SDL_free(pNewJoystick->joystickname);
        SDL_free(pNewJoystick);
        return DIENUM_CONTINUE;
    }
#endif

#ifdef SDL_JOYSTICK_RAWINPUT
    if (RAWINPUT_IsDevicePresent(vendor, product, 0, pNewJoystick->joystickname)) {
        /* The RAWINPUT driver is taking care of this device */
        SDL_free(pNewJoystick);
        return DIENUM_CONTINUE;
    }
#endif

    WINDOWS_AddJoystickDevice(pNewJoystick);

    return DIENUM_CONTINUE; /* get next device, please */
}

void
SDL_DINPUT_JoystickDetect(JoyStick_DeviceData **pContext)
{
    IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, pContext, DIEDFL_ATTACHEDONLY);

    if (SDL_RawDevList) {
        SDL_free(SDL_RawDevList);  /* in case we used this in DirectInput detection */
        SDL_RawDevList = NULL;
    }
    SDL_RawDevListCount = 0;
}

typedef struct
{
    Uint16 vendor;
    Uint16 product;
    Uint16 version;
    SDL_bool present;
} EnumJoystickPresentData;

static BOOL CALLBACK
EnumJoystickPresentCallback(const DIDEVICEINSTANCE * pdidInstance, VOID * pContext)
{
    EnumJoystickPresentData *data = (EnumJoystickPresentData *)pContext;
    Uint16 vendor = 0;
    Uint16 product = 0;
    Uint16 version = 0;

    if (SDL_memcmp(&pdidInstance->guidProduct.Data4[2], "PIDVID", 6) == 0) {
        vendor = (Uint16)LOWORD(pdidInstance->guidProduct.Data1);
        product = (Uint16)HIWORD(pdidInstance->guidProduct.Data1);
        if (data->vendor == vendor && data->product == product && data->version == version) {
            data->present = SDL_TRUE;
            return DIENUM_STOP;
        }
    }
    return DIENUM_CONTINUE;
}

SDL_bool
SDL_DINPUT_JoystickPresent(Uint16 vendor, Uint16 product, Uint16 version)
{
    EnumJoystickPresentData data;

    if (dinput == NULL) {
        return SDL_FALSE;
    }

    data.vendor = vendor;
    data.product = product;
    data.version = version;
    data.present = SDL_FALSE;
    IDirectInput8_EnumDevices(dinput, DI8DEVCLASS_GAMECTRL, EnumJoystickPresentCallback, &data, DIEDFL_ATTACHEDONLY);

    return data.present;
}

static BOOL CALLBACK
EnumDevObjectsCallback(LPCDIDEVICEOBJECTINSTANCE dev, LPVOID pvRef)
{
    SDL_Joystick *joystick = (SDL_Joystick *)pvRef;
    HRESULT result;
    input_t *in = &joystick->hwdata->Inputs[joystick->hwdata->NumInputs];

    if (dev->dwType & DIDFT_BUTTON) {
        in->type = BUTTON;
        in->num = joystick->nbuttons;
        in->ofs = DIJOFS_BUTTON(in->num);
        joystick->nbuttons++;
    } else if (dev->dwType & DIDFT_POV) {
        in->type = HAT;
        in->num = joystick->nhats;
        in->ofs = DIJOFS_POV(in->num);
        joystick->nhats++;
    } else if (dev->dwType & DIDFT_AXIS) {
        DIPROPRANGE diprg;
        DIPROPDWORD dilong;

        in->type = AXIS;
        in->num = joystick->naxes;
        if (!SDL_memcmp(&dev->guidType, &GUID_XAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_X;
        else if (!SDL_memcmp(&dev->guidType, &GUID_YAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_Y;
        else if (!SDL_memcmp(&dev->guidType, &GUID_ZAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_Z;
        else if (!SDL_memcmp(&dev->guidType, &GUID_RxAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_RX;
        else if (!SDL_memcmp(&dev->guidType, &GUID_RyAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_RY;
        else if (!SDL_memcmp(&dev->guidType, &GUID_RzAxis, sizeof(dev->guidType)))
            in->ofs = DIJOFS_RZ;
        else if (!SDL_memcmp(&dev->guidType, &GUID_Slider, sizeof(dev->guidType))) {
            in->ofs = DIJOFS_SLIDER(joystick->hwdata->NumSliders);
            ++joystick->hwdata->NumSliders;
        } else {
            return DIENUM_CONTINUE; /* not an axis we can grok */
        }

        diprg.diph.dwSize = sizeof(diprg);
        diprg.diph.dwHeaderSize = sizeof(diprg.diph);
        diprg.diph.dwObj = dev->dwType;
        diprg.diph.dwHow = DIPH_BYID;
        diprg.lMin = SDL_JOYSTICK_AXIS_MIN;
        diprg.lMax = SDL_JOYSTICK_AXIS_MAX;

        result =
            IDirectInputDevice8_SetProperty(joystick->hwdata->InputDevice,
            DIPROP_RANGE, &diprg.diph);
        if (FAILED(result)) {
            return DIENUM_CONTINUE;     /* don't use this axis */
        }

        /* Set dead zone to 0. */
        dilong.diph.dwSize = sizeof(dilong);
        dilong.diph.dwHeaderSize = sizeof(dilong.diph);
        dilong.diph.dwObj = dev->dwType;
        dilong.diph.dwHow = DIPH_BYID;
        dilong.dwData = 0;
        result =
            IDirectInputDevice8_SetProperty(joystick->hwdata->InputDevice,
            DIPROP_DEADZONE, &dilong.diph);
        if (FAILED(result)) {
            return DIENUM_CONTINUE;     /* don't use this axis */
        }

        joystick->naxes++;
    } else {
        /* not supported at this time */
        return DIENUM_CONTINUE;
    }

    joystick->hwdata->NumInputs++;

    if (joystick->hwdata->NumInputs == MAX_INPUTS) {
        return DIENUM_STOP;     /* too many */
    }

    return DIENUM_CONTINUE;
}

/* Sort using the data offset into the DInput struct.
 * This gives a reasonable ordering for the inputs.
 */
static int
SortDevFunc(const void *a, const void *b)
{
    const input_t *inputA = (const input_t*)a;
    const input_t *inputB = (const input_t*)b;

    if (inputA->ofs < inputB->ofs)
        return -1;
    if (inputA->ofs > inputB->ofs)
        return 1;
    return 0;
}

/* Sort the input objects and recalculate the indices for each input. */
static void
SortDevObjects(SDL_Joystick *joystick)
{
    input_t *inputs = joystick->hwdata->Inputs;
    int nButtons = 0;
    int nHats = 0;
    int nAxis = 0;
    int n;

    SDL_qsort(inputs, joystick->hwdata->NumInputs, sizeof(input_t), SortDevFunc);

    for (n = 0; n < joystick->hwdata->NumInputs; n++) {
        switch (inputs[n].type) {
        case BUTTON:
            inputs[n].num = nButtons;
            nButtons++;
            break;

        case HAT:
            inputs[n].num = nHats;
            nHats++;
            break;

        case AXIS:
            inputs[n].num = nAxis;
            nAxis++;
            break;
        }
    }
}

int
SDL_DINPUT_JoystickOpen(SDL_Joystick * joystick, JoyStick_DeviceData *joystickdevice)
{
    HRESULT result;
    LPDIRECTINPUTDEVICE8 device;
    DIPROPDWORD dipdw;

    joystick->hwdata->buffered = SDL_TRUE;
    joystick->hwdata->Capabilities.dwSize = sizeof(DIDEVCAPS);

    SDL_zero(dipdw);
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);

    result =
        IDirectInput8_CreateDevice(dinput,
        &(joystickdevice->dxdevice.guidInstance), &device, NULL);
    if (FAILED(result)) {
        return SetDIerror("IDirectInput::CreateDevice", result);
    }

    /* Now get the IDirectInputDevice8 interface, instead. */
    result = IDirectInputDevice8_QueryInterface(device,
        &IID_IDirectInputDevice8,
        (LPVOID *)& joystick->
        hwdata->InputDevice);
    /* We are done with this object.  Use the stored one from now on. */
    IDirectInputDevice8_Release(device);

    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::QueryInterface", result);
    }

    /* Acquire shared access. Exclusive access is required for forces,
    * though. */
    result =
        IDirectInputDevice8_SetCooperativeLevel(joystick->hwdata->
        InputDevice, SDL_HelperWindow,
        DISCL_EXCLUSIVE |
        DISCL_BACKGROUND);
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SetCooperativeLevel", result);
    }

    /* Use the extended data structure: DIJOYSTATE2. */
    result =
        IDirectInputDevice8_SetDataFormat(joystick->hwdata->InputDevice,
        &SDL_c_dfDIJoystick2);
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SetDataFormat", result);
    }

    /* Get device capabilities */
    result =
        IDirectInputDevice8_GetCapabilities(joystick->hwdata->InputDevice,
        &joystick->hwdata->Capabilities);
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::GetCapabilities", result);
    }

    /* Force capable? */
    if (joystick->hwdata->Capabilities.dwFlags & DIDC_FORCEFEEDBACK) {
        result = IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        if (FAILED(result)) {
            return SetDIerror("IDirectInputDevice8::Acquire", result);
        }

        /* reset all actuators. */
        result =
            IDirectInputDevice8_SendForceFeedbackCommand(joystick->hwdata->
            InputDevice,
            DISFFC_RESET);

        /* Not necessarily supported, ignore if not supported.
        if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SendForceFeedbackCommand", result);
        }
        */

        result = IDirectInputDevice8_Unacquire(joystick->hwdata->InputDevice);

        if (FAILED(result)) {
            return SetDIerror("IDirectInputDevice8::Unacquire", result);
        }

        /* Turn on auto-centering for a ForceFeedback device (until told
        * otherwise). */
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DIPROPAUTOCENTER_ON;

        result =
            IDirectInputDevice8_SetProperty(joystick->hwdata->InputDevice,
            DIPROP_AUTOCENTER, &dipdw.diph);

        /* Not necessarily supported, ignore if not supported.
        if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SetProperty", result);
        }
        */
    }

    /* What buttons and axes does it have? */
    IDirectInputDevice8_EnumObjects(joystick->hwdata->InputDevice,
        EnumDevObjectsCallback, joystick,
        DIDFT_BUTTON | DIDFT_AXIS | DIDFT_POV);

    /* Reorder the input objects. Some devices do not report the X axis as
    * the first axis, for example. */
    SortDevObjects(joystick);

    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = INPUT_QSIZE;

    /* Set the buffer size */
    result =
        IDirectInputDevice8_SetProperty(joystick->hwdata->InputDevice,
        DIPROP_BUFFERSIZE, &dipdw.diph);

    if (result == DI_POLLEDDEVICE) {
        /* This device doesn't support buffering, so we're forced
         * to use less reliable polling. */
        joystick->hwdata->buffered = SDL_FALSE;
    } else if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SetProperty", result);
    }
    return 0;
}

static int
SDL_DINPUT_JoystickInitRumble(SDL_Joystick * joystick, Sint16 magnitude)
{
    HRESULT result;

    /* Reset and then enable actuators */
    result = IDirectInputDevice8_SendForceFeedbackCommand(joystick->hwdata->InputDevice, DISFFC_RESET);
    if (result == DIERR_INPUTLOST || result == DIERR_NOTEXCLUSIVEACQUIRED) {
        result = IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        if (SUCCEEDED(result)) {
            result = IDirectInputDevice8_SendForceFeedbackCommand(joystick->hwdata->InputDevice, DISFFC_RESET);
        }
    }
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SendForceFeedbackCommand(DISFFC_RESET)", result);
    }

    result = IDirectInputDevice8_SendForceFeedbackCommand(joystick->hwdata->InputDevice, DISFFC_SETACTUATORSON);
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::SendForceFeedbackCommand(DISFFC_SETACTUATORSON)", result);
    }

    /* Create the effect */
    joystick->hwdata->ffeffect = CreateRumbleEffectData(magnitude);
    if (!joystick->hwdata->ffeffect) {
        return SDL_OutOfMemory();
    }

    result = IDirectInputDevice8_CreateEffect(joystick->hwdata->InputDevice, &GUID_Sine,
                                              joystick->hwdata->ffeffect, &joystick->hwdata->ffeffect_ref, NULL);
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::CreateEffect", result);
    }
    return 0;
}

int
SDL_DINPUT_JoystickRumble(SDL_Joystick * joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
{
    HRESULT result;

    /* Scale and average the two rumble strengths */
    Sint16 magnitude = (Sint16)(((low_frequency_rumble / 2) + (high_frequency_rumble / 2)) / 2);

    if (!(joystick->hwdata->Capabilities.dwFlags & DIDC_FORCEFEEDBACK)) {
        return SDL_Unsupported();
    }

    if (joystick->hwdata->ff_initialized) {
        DIPERIODIC *periodic = ((DIPERIODIC *)joystick->hwdata->ffeffect->lpvTypeSpecificParams);
        periodic->dwMagnitude = CONVERT_MAGNITUDE(magnitude);

        result = IDirectInputEffect_SetParameters(joystick->hwdata->ffeffect_ref, joystick->hwdata->ffeffect, (DIEP_DURATION | DIEP_TYPESPECIFICPARAMS));
        if (result == DIERR_INPUTLOST) {
            result = IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
            if (SUCCEEDED(result)) {
                result = IDirectInputEffect_SetParameters(joystick->hwdata->ffeffect_ref, joystick->hwdata->ffeffect, (DIEP_DURATION | DIEP_TYPESPECIFICPARAMS));
            }
        }
        if (FAILED(result)) {
            return SetDIerror("IDirectInputDevice8::SetParameters", result);
        }
    } else {
        if (SDL_DINPUT_JoystickInitRumble(joystick, magnitude) < 0) {
            return -1;
        }
        joystick->hwdata->ff_initialized = SDL_TRUE;
    }

    result = IDirectInputEffect_Start(joystick->hwdata->ffeffect_ref, 1, 0);
    if (result == DIERR_INPUTLOST || result == DIERR_NOTEXCLUSIVEACQUIRED) {
        result = IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        if (SUCCEEDED(result)) {
            result = IDirectInputEffect_Start(joystick->hwdata->ffeffect_ref, 1, 0);
        }
    }
    if (FAILED(result)) {
        return SetDIerror("IDirectInputDevice8::Start", result);
    }
    return 0;
}

static Uint8
TranslatePOV(DWORD value)
{
    const int HAT_VALS[] = {
        SDL_HAT_UP,
        SDL_HAT_UP | SDL_HAT_RIGHT,
        SDL_HAT_RIGHT,
        SDL_HAT_DOWN | SDL_HAT_RIGHT,
        SDL_HAT_DOWN,
        SDL_HAT_DOWN | SDL_HAT_LEFT,
        SDL_HAT_LEFT,
        SDL_HAT_UP | SDL_HAT_LEFT
    };

    if (LOWORD(value) == 0xFFFF)
        return SDL_HAT_CENTERED;

    /* Round the value up: */
    value += 4500 / 2;
    value %= 36000;
    value /= 4500;

    if (value >= 8)
        return SDL_HAT_CENTERED;        /* shouldn't happen */

    return HAT_VALS[value];
}

static void
UpdateDINPUTJoystickState_Buffered(SDL_Joystick * joystick)
{
    int i;
    HRESULT result;
    DWORD numevents;
    DIDEVICEOBJECTDATA evtbuf[INPUT_QSIZE];

    numevents = INPUT_QSIZE;
    result =
        IDirectInputDevice8_GetDeviceData(joystick->hwdata->InputDevice,
        sizeof(DIDEVICEOBJECTDATA), evtbuf,
        &numevents, 0);
    if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
        IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        result =
            IDirectInputDevice8_GetDeviceData(joystick->hwdata->InputDevice,
            sizeof(DIDEVICEOBJECTDATA),
            evtbuf, &numevents, 0);
    }

    /* Handle the events or punt */
    if (FAILED(result)) {
        return;
    }

    for (i = 0; i < (int)numevents; ++i) {
        int j;

        for (j = 0; j < joystick->hwdata->NumInputs; ++j) {
            const input_t *in = &joystick->hwdata->Inputs[j];

            if (evtbuf[i].dwOfs != in->ofs)
                continue;

            switch (in->type) {
            case AXIS:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)evtbuf[i].dwData);
                break;
            case BUTTON:
                SDL_PrivateJoystickButton(joystick, in->num,
                    (Uint8)(evtbuf[i].dwData ? SDL_PRESSED : SDL_RELEASED));
                break;
            case HAT:
                {
                    Uint8 pos = TranslatePOV(evtbuf[i].dwData);
                    SDL_PrivateJoystickHat(joystick, in->num, pos);
                }
                break;
            }
        }
    }
}

/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */
static void
UpdateDINPUTJoystickState_Polled(SDL_Joystick * joystick)
{
    DIJOYSTATE2 state;
    HRESULT result;
    int i;

    result =
        IDirectInputDevice8_GetDeviceState(joystick->hwdata->InputDevice,
        sizeof(DIJOYSTATE2), &state);
    if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
        IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        result =
            IDirectInputDevice8_GetDeviceState(joystick->hwdata->InputDevice,
            sizeof(DIJOYSTATE2), &state);
    }

    if (result != DI_OK) {
        return;
    }

    /* Set each known axis, button and POV. */
    for (i = 0; i < joystick->hwdata->NumInputs; ++i) {
        const input_t *in = &joystick->hwdata->Inputs[i];

        switch (in->type) {
        case AXIS:
            switch (in->ofs) {
            case DIJOFS_X:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lX);
                break;
            case DIJOFS_Y:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lY);
                break;
            case DIJOFS_Z:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lZ);
                break;
            case DIJOFS_RX:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lRx);
                break;
            case DIJOFS_RY:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lRy);
                break;
            case DIJOFS_RZ:
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.lRz);
                break;
            case DIJOFS_SLIDER(0):
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.rglSlider[0]);
                break;
            case DIJOFS_SLIDER(1):
                SDL_PrivateJoystickAxis(joystick, in->num, (Sint16)state.rglSlider[1]);
                break;
            }
            break;

        case BUTTON:
            SDL_PrivateJoystickButton(joystick, in->num,
                (Uint8)(state.rgbButtons[in->ofs - DIJOFS_BUTTON0] ? SDL_PRESSED : SDL_RELEASED));
            break;
        case HAT:
        {
            Uint8 pos = TranslatePOV(state.rgdwPOV[in->ofs - DIJOFS_POV(0)]);
            SDL_PrivateJoystickHat(joystick, in->num, pos);
            break;
        }
        }
    }
}

void
SDL_DINPUT_JoystickUpdate(SDL_Joystick * joystick)
{
    HRESULT result;

    result = IDirectInputDevice8_Poll(joystick->hwdata->InputDevice);
    if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
        IDirectInputDevice8_Acquire(joystick->hwdata->InputDevice);
        IDirectInputDevice8_Poll(joystick->hwdata->InputDevice);
    }

    if (joystick->hwdata->buffered) {
        UpdateDINPUTJoystickState_Buffered(joystick);
    } else {
        UpdateDINPUTJoystickState_Polled(joystick);
    }
}

void
SDL_DINPUT_JoystickClose(SDL_Joystick * joystick)
{
    if (joystick->hwdata->ffeffect_ref) {
        IDirectInputEffect_Unload(joystick->hwdata->ffeffect_ref);
        joystick->hwdata->ffeffect_ref = NULL;
    }
    if (joystick->hwdata->ffeffect) {
        FreeRumbleEffectData(joystick->hwdata->ffeffect);
        joystick->hwdata->ffeffect = NULL;
    }
    IDirectInputDevice8_Unacquire(joystick->hwdata->InputDevice);
    IDirectInputDevice8_Release(joystick->hwdata->InputDevice);
    joystick->hwdata->ff_initialized = SDL_FALSE;
}

void
SDL_DINPUT_JoystickQuit(void)
{
    if (dinput != NULL) {
        IDirectInput8_Release(dinput);
        dinput = NULL;
    }

    if (coinitialized) {
        WIN_CoUninitialize();
        coinitialized = SDL_FALSE;
    }
}

#else /* !SDL_JOYSTICK_DINPUT */

typedef struct JoyStick_DeviceData JoyStick_DeviceData;

int
SDL_DINPUT_JoystickInit(void)
{
    return 0;
}

void
SDL_DINPUT_JoystickDetect(JoyStick_DeviceData **pContext)
{
}

SDL_bool
SDL_DINPUT_JoystickPresent(Uint16 vendor, Uint16 product, Uint16 version)
{
    return SDL_FALSE;
}

int
SDL_DINPUT_JoystickOpen(SDL_Joystick * joystick, JoyStick_DeviceData *joystickdevice)
{
    return SDL_Unsupported();
}

int
SDL_DINPUT_JoystickRumble(SDL_Joystick * joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
{
    return SDL_Unsupported();
}

void
SDL_DINPUT_JoystickUpdate(SDL_Joystick * joystick)
{
}

void
SDL_DINPUT_JoystickClose(SDL_Joystick * joystick)
{
}

void
SDL_DINPUT_JoystickQuit(void)
{
}

#endif /* SDL_JOYSTICK_DINPUT */

/* vi: set ts=4 sw=4 expandtab: */
