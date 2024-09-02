#pragma once

#include "guilite/api.h"
#include "guilite/wnd.h"
#include "guilite/resource.h"
#include "guilite/word.h"
#include "guilite/display.h"
#include "guilite/theme.h"

#include "guilite/keyboard.h"

#define MAX_EDIT_STRLEN 32
#define IDD_KEY_BOARD   0x1

typedef struct c_edit c_edit;
struct c_edit
{
    c_wnd base;
    KEYBOARD_STYLE m_kb_style;
    char m_str_input[MAX_EDIT_STRLEN];
    char m_str[MAX_EDIT_STRLEN];
};

const char *c_edit_get_text(c_edit *self);
void c_edit_set_text(c_edit *self, const char *str);
void c_edit_set_keyboard_style(c_edit *self, KEYBOARD_STYLE kb_sytle);
void c_edit_vir_pre_create_wnd(c_wnd *self);
void c_edit_vir_on_paint(c_wnd *self);
void c_edit_vir_on_focus(c_wnd *self);
void c_edit_vir_on_kill_focus(c_wnd *self);
void c_edit_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key);
void c_edit_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_edit_on_key_board_click(c_wnd *self, int id, int param);
void c_edit_on_touch_down(c_edit *self, int x, int y);
void c_edit_on_touch_up(c_edit *self, int x, int y);
void c_edit_init(c_edit *self);
