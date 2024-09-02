#pragma once

#include "guilite/button.h"
#include "guilite/dialog.h"
#include "guilite/keyboard.h"

extern DIALOG_ARRAY c_dialog_ms_the_dialogs[SURFACE_CNT_MAX];
extern c_keyboard c_edit_s_keyboard;

extern WND_TREE g_key_board_children[];
extern WND_TREE g_number_board_children[];

void c_widgets_init(void);
