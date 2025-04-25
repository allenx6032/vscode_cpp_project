/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_FAUR_H
#define F_INC_FAUR_H

#ifdef __cplusplus
#define F_EXTERN_C_START extern "C" {
#define F_EXTERN_C_END }
#else
#define F_EXTERN_C_START
#define F_EXTERN_C_END
#endif

F_EXTERN_C_START
#include "faur_collide.p.h"
#include "faur_grid.p.h"
#include "faur_bitfield.p.h"
#include "faur_block.p.h"
#include "faur_hash.p.h"
#include "faur_list.p.h"
#include "faur_listintr.p.h"
#include "faur_collection.p.h"
#include "faur_component.p.h"
#include "faur_ecs.p.h"
#include "faur_entity.p.h"
#include "faur_system.p.h"
#include "faur_blob.p.h"
#include "faur_dir.p.h"
#include "faur_file.p.h"
#include "faur_path.p.h"
#include "faur_console.p.h"
#include "faur_errors.p.h"
#include "faur_fps.p.h"
#include "faur_init.p.h"
#include "faur_main.p.h"
#include "faur_menu.p.h"
#include "faur_out.p.h"
#include "faur_state.p.h"
#include "faur_align.p.h"
#include "faur_color.p.h"
#include "faur_draw.p.h"
#include "faur_fade.p.h"
#include "faur_font.p.h"
#include "faur_screen.p.h"
#include "faur_screenshot.p.h"
#include "faur_sprite.p.h"
#include "faur_spritelayers.p.h"
#include "faur_spritesheet.p.h"
#include "faur_analog.p.h"
#include "faur_button.p.h"
#include "faur_controller.p.h"
#include "faur_input.p.h"
#include "faur_touch.p.h"
#include "faur_fix.p.h"
#include "faur_math.p.h"
#include "faur_random.p.h"
#include "faur_vec.p.h"
#include "faur_mem.p.h"
#include "faur_pool.p.h"
#include "faur_gamebuino_video.p.h"
#include "faur_channel.p.h"
#include "faur_music.p.h"
#include "faur_sample.p.h"
#include "faur_str.p.h"
#include "faur_time.p.h"
#include "faur_timer.p.h"
F_EXTERN_C_END

#ifndef F_INC_FAUR_V_H
#include <faur_v/include.h>
#endif

#endif // F_INC_FAUR_H
