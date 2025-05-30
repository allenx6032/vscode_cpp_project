/* Deccan Game Engine - C11 2D SDL Game Engine.
 * Copyright 2020, 2021 Ayush Bardhan Tripathy
 *
 * This software is licensed under MIT License.
 * See LICENSE.md included with this package for more info.
 */

#pragma once
#include "deccan_config.h"

////////////////////////////////////////////////////////////////////////////////
// Enums
////////////////////////////////////////////////////////////////////////////////

// TODO: Incomplete
typedef enum {
    DECCAN_KEY_UNKNOWN1 = 0,

    DECCAN_KEY_A = 4,
    DECCAN_KEY_B,
    DECCAN_KEY_C,
    DECCAN_KEY_D,
    DECCAN_KEY_E,
    DECCAN_KEY_F,
    DECCAN_KEY_G,
    DECCAN_KEY_H,
    DECCAN_KEY_I,
    DECCAN_KEY_J,
    DECCAN_KEY_K,
    DECCAN_KEY_L,
    DECCAN_KEY_M,
    DECCAN_KEY_N,
    DECCAN_KEY_O,
    DECCAN_KEY_P,
    DECCAN_KEY_Q,
    DECCAN_KEY_R,
    DECCAN_KEY_S,
    DECCAN_KEY_T,
    DECCAN_KEY_U,
    DECCAN_KEY_V,
    DECCAN_KEY_W,
    DECCAN_KEY_X,
    DECCAN_KEY_Y,
    DECCAN_KEY_Z,

    DECCAN_KEY_NUM0,
    DECCAN_KEY_NUM1,
    DECCAN_KEY_NUM2,
    DECCAN_KEY_NUM3,
    DECCAN_KEY_NUM4,
    DECCAN_KEY_NUM5,
    DECCAN_KEY_NUM6,
    DECCAN_KEY_NUM7,
    DECCAN_KEY_NUM8,
    DECCAN_KEY_NUM9,

    DECCAN_KEY_ENTER,
    DECCAN_KEY_ESCAPE,
    DECCAN_KEY_BACKSPACE,
    DECCAN_KEY_TAB,
    DECCAN_KEY_SPACE,
    DECCAN_KEY_MINUS,
    DECCAN_KEY_EQUALS,
    DECCAN_KEY_LEFTBRAC,
    DECCAN_KEY_RIGHTBRAC,
    DECCAN_KEY_BACKSLASH,
    DECCAN_KEY_UNKNOWN2,
    DECCAN_KEY_SEMICOLON,
    DECCAN_KEY_APOSTROPHE,
    DECCAN_KEY_GRAVE,
    DECCAN_KEY_COMMA,
    DECCAN_KEY_PERIOD,
    DECCAN_KEY_SLASH,
    DECCAN_KEY_CAPSLOCK,

    DECCAN_KEY_F1,
    DECCAN_KEY_F2,
    DECCAN_KEY_F3,
    DECCAN_KEY_F4,
    DECCAN_KEY_F5,
    DECCAN_KEY_F6,
    DECCAN_KEY_F7,
    DECCAN_KEY_F8,
    DECCAN_KEY_F9,
    DECCAN_KEY_F10,
    DECCAN_KEY_F11,
    DECCAN_KEY_F12,

    DECCAN_KEY_PRINTSCREEN,
    DECCAN_KEY_SCROLLLOCK,
    DECCAN_KEY_PAUSE,
    DECCAN_KEY_INSERT,
    DECCAN_KEY_HOME,
    DECCAN_KEY_PAGEUP,
    DECCAN_KEY_DELETE,
    DECCAN_KEY_END,
    DECCAN_KEY_PAGEDOWN,
    DECCAN_KEY_RIGHT,
    DECCAN_KEY_LEFT,
    DECCAN_KEY_DOWN,
    DECCAN_KEY_UP,

    DECCAN_KEY_TOTAL_COUNT
} deccan_key_code_t;

typedef enum {
    DECCAN_BUTTON_LEFT,
    DECCAN_BUTTON_MIDDLE,
    DECCAN_BUTTON_RIGHT,
    DECCAN_BUTTON_X1,
    DECCAN_BUTTON_X2,

    DECCAN_BUTTON_TOTAL_COUNT
} deccan_button_code_t;
