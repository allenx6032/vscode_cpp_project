#pragma once

inline static const char* kVirtualKeyCodes[] = {
    "Undefined",    // 0
    "Alpha0",       // 1
    "Alpha1",       // 2
    "Alpha2",       // 3
    "Alpha3",       // 4
    "Alpha4",       // 5
    "Alpha5",       // 6
    "Alpha6",       // 7
    "Alpha7",       // 8
    "Alpha8",       // 9
    "Alpha9",       // 10

    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", // 11 - 20
    "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", // 21 - 30
    "U", "V", "W", "X", "Y", "Z",                    // 31 - 36

    "F1", "F2", "F3", "F4", "F5", "F6",               // 37 - 42
    "F7", "F8", "F9", "F10", "F11", "F12",            // 43 - 48
    "F13", "F14", "F15", "F16", "F17", "F18",         // 49 - 54
    "F19", "F20", "F21", "F22", "F23", "F24",         // 55 - 60

    "Keypad0",      // 61
    "Keypad1",      // 62
    "Keypad2",      // 63
    "Keypad3",      // 64
    "Keypad4",      // 65
    "Keypad5",      // 66
    "Keypad6",      // 67
    "Keypad7",      // 68
    "Keypad8",      // 69
    "Keypad9",      // 70

    "KeypadClear",  // 71
    "KeypadDecimal",// 72
    "KeypadDivide", // 73
    "KeypadEnter",  // 74
    "KeypadEquals", // 75
    "KeypadMinus",  // 76
    "KeypadMultiply",//77
    "KeypadPlus",   // 78

    "Backslash",    // 79
    "Comma",        // 80
    "Equals",       // 81
    "Grave",        // 82
    "LeftBracket",  // 83
    "Minus",        // 84
    "Period",       // 85
    "Quote",        // 86
    "RightBracket", // 87
    "Semicolon",    // 88
    "Slash",        // 89

    "Meta",         // 90
    "Ctrl",         // 91
    "Function",     // 92
    "Alt",          // 93
    "Shift",        // 94
    "RightMeta",    // 95
    "RightCtrl",    // 96
    "RightAlt",     // 97
    "RightShift",   // 98
    "CapsLock",     // 99
    "NumLock",      // 100
    "ScrollLock",   // 101

    "DownArrow",    // 102
    "LeftArrow",    // 103
    "RightArrow",   // 104
    "UpArrow",      // 105

    "End",          // 106
    "Home",         // 107
    "PageDown",     // 108
    "PageUp",       // 109

    "ContextMenu",  // 110
    "Backspace",    // 111
    "Escape",       // 112
    "Delete",       // 113
    "Help",         // 114
    "Mute",         // 115
    "PrintScreen",  // 116
    "Enter",        // 117
    "Space",        // 118
    "Tab",          // 119
    "VolumeDown",   // 120
    "VolumeUp",     // 121
    "Pause",        // 122
    "Insert",       // 123
    "Sleep",        // 124
};

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const int _native_key_mapping[] = {
    0,

    // numbers
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,

    // letters
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4D,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,

    // function
    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,
    VK_F13,
    VK_F14,
    VK_F15,
    VK_F16,
    VK_F17,
    VK_F18,
    VK_F19,
    VK_F20,
    VK_F21,
    VK_F22,
    VK_F23,
    VK_F24,

    // keypad numbers
    VK_NUMPAD0,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,

    // keypad other
    VK_CLEAR,
    VK_DECIMAL,
    VK_DIVIDE,
    VK_RETURN, // KeypadEnter
    0,         // KeypadEquals
    VK_SUBTRACT,
    VK_MULTIPLY,
    VK_ADD,

    // others
    VK_OEM_5, // Backslash
    VK_OEM_COMMA,
    VK_OEM_PLUS, // Equal
    VK_OEM_3,    // Grave
    VK_OEM_4,    // LeftBracket
    VK_OEM_MINUS,
    VK_OEM_PERIOD,
    VK_OEM_7, // Quote
    VK_OEM_6, // RightBracket
    VK_OEM_1, // Semicolon
    VK_OEM_2, // Slash

    // modifiers
    VK_LWIN,
    VK_CONTROL,
    0, // Function
    VK_MENU,
    VK_SHIFT,
    VK_RWIN,
    VK_RCONTROL,
    VK_RMENU,
    VK_RSHIFT,
    VK_CAPITAL,
    VK_NUMLOCK,
    VK_SCROLL,

    // arrows
    VK_DOWN,
    VK_LEFT,
    VK_RIGHT,
    VK_UP,

    // navigation
    VK_END,
    VK_HOME,
    VK_PRIOR,
    VK_NEXT,

    // actions
    VK_APPS, // ContextMenu
    VK_BACK, // Delete
    VK_ESCAPE,
    VK_DELETE, // ForwardDelete
    VK_HELP,
    VK_VOLUME_MUTE,
    VK_SNAPSHOT, // PrintScreen
    VK_RETURN,
    VK_SPACE,
    VK_TAB,
    VK_VOLUME_DOWN,
    VK_VOLUME_UP,
    VK_PAUSE,
    VK_INSERT,
    VK_SLEEP,
};

inline bool _platform_get_key(int scancode) {
    return GetKeyState(scancode) & 0x8000;
}

#elif __linux__ && !__ANDROID__
#include <X11/Xlib.h>
#include "X11/keysym.h"

#undef Success
#undef None

const int _native_key_mapping[] = {
    0,

    // numbers
    XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8, XK_9,

    // letters
    XK_a, XK_b, XK_c, XK_d, XK_e, XK_f, XK_g, XK_h, XK_i, XK_j, XK_k, XK_l, XK_m, XK_n, XK_o, XK_p,
    XK_q, XK_r, XK_s, XK_t, XK_u, XK_v, XK_w, XK_x, XK_y, XK_z,

    // function
    XK_F1, XK_F2, XK_F3, XK_F4, XK_F5, XK_F6, XK_F7, XK_F8, XK_F9, XK_F10, XK_F11, XK_F12, XK_F13,
    XK_F14, XK_F15, XK_F16, XK_F17, XK_F18, XK_F19, XK_F20, XK_F21, XK_F22, XK_F23, XK_F24,

    // keypad numbers
    XK_KP_0, XK_KP_1, XK_KP_2, XK_KP_3, XK_KP_4, XK_KP_5, XK_KP_6, XK_KP_7, XK_KP_8, XK_KP_9,

    // keypad other
    0, // KeypadClear
    XK_KP_Decimal, XK_KP_Divide, XK_KP_Enter, XK_KP_Equal, XK_KP_Subtract, XK_KP_Multiply,
    XK_KP_Add,

    // others
    XK_backslash, XK_comma, XK_equal, XK_grave, XK_bracketleft, XK_minus, XK_period, XK_apostrophe,
    XK_bracketright, XK_semicolon, XK_slash,

    // modifiers
    XK_Super_L, // Meta
    XK_Control_L,
    0, // Function
    XK_Alt_L, XK_Shift_L,
    XK_Super_R, // RightMeta
    XK_Control_R, XK_Alt_R, XK_Shift_R, XK_Caps_Lock, XK_Num_Lock, XK_Scroll_Lock,

    // arrows
    XK_Down, XK_Left, XK_Right, XK_Up,

    // navigation
    XK_End, XK_Home, XK_Page_Down, XK_Page_Up,

    // actions
    XK_Menu, XK_BackSpace, XK_Escape, XK_Delete, XK_Help,
    0, // Mute
    0, // PrintScreen
    XK_Return, XK_space, XK_Tab,
    0, // VolumeDown
    0, // VolumeUp
    XK_Pause, XK_Insert,
    0, // Sleep
};

inline bool _platform_get_key(int scancode) {
    static Display *dpy = XOpenDisplay(":0");
    static char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, scancode);
    bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    // XCloseDisplay(dpy);
    return isPressed;
}
#elif __ANDROID__
const int _native_key_mapping[sizeof(kVirtualKeyCodes)/sizeof(char*)] = {};
inline bool _platform_get_key(int scancode) { return false; }

#elif __EMSCRIPTEN__
#include <emscripten.h>

const int _native_key_mapping[sizeof(kVirtualKeyCodes)/sizeof(char*)] = {
    0,

    // numbers
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    // letters
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

    // function
    1082, 1083, 1084, 1085, 1086, 1087,
    1088, 1089, 1090, 1091, 1092, 1093,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,

    // keypad numbers
    1122, 1113, 1114, 1115, 1116,
    1117, 1118, 1119, 1120, 1121,

    // keypad other
    0, // KeypadClear
    0,
    0,
    0,
    0,
    0,
    0,
    0,

    // others
    92, 44, 187, 96, 91, 189, 45, 39, 93, 186, 47,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0,

    // arrows
    1105, 1104, 1103, 1106,

    // navigation
    0, 0, 0, 0,

    // actions
    0, 8, 27, 127, 0,
    0, 0, 13, 32, 9,
    0, 0, 0, 0, 0
};
inline bool _platform_get_key(int scancode) { return false; }

#include <map>
inline std::map<int, int> _scancode_to_keycode_map = [](){
    std::map<int, int> map;
    for(int i = 0; i < sizeof(_native_key_mapping)/sizeof(int); i++){
        int scancode = _native_key_mapping[i];
        if(scancode == 0) continue;
        map[scancode] = i;
    }
    return map;
}();

#else
#error "Unsupported platform"
#endif

const int KEY_MAPPING_SIZE = sizeof(_native_key_mapping) / sizeof(int);
static_assert(KEY_MAPPING_SIZE == sizeof(kVirtualKeyCodes)/sizeof(char*));

inline int keycode_to_scancode(int code){
    if(code < 0 || code >= KEY_MAPPING_SIZE) return 0;
    return _native_key_mapping[code];
}