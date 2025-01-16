/**
*** :: Corange ::
***
***   Pure and Simple game engine written in C 
***   
***   Uses SDL as a bottom layer and OpenGL for rendering
***   Provides asset, UI and entity management.
***   As well as deferred and forward renderers.
***   And a host of small demos.
***
***   Suggestions and contributions welcome:
***
***     Daniel Holden | contact@theorangeduck.com
***     
***     https://github.com/orangeduck/Corange
***
**/

#ifndef corange_h
#define corange_h

/* Core engine modules */

#include "corange_cengine.h"
#include "corange_cgraphics.h"
#include "corange_caudio.h"
#include "corange_cjoystick.h"
#include "corange_cnet.h"
#include "corange_cphysics.h"

/* Corange Functions */

void corange_init(const char* core_assets_path);
void corange_finish();

/* Entities */

#include "corange_centity.h"

#include "corange_camera.h"
#include "corange_light.h"
#include "corange_static_object.h"
#include "corange_animated_object.h"
#include "corange_physics_object.h"
#include "corange_instance_object.h"
#include "corange_landscape.h"
#include "corange_particles.h"

/* Assets */

#include "corange_casset.h"

#include "corange_config.h"
#include "corange_image.h"
#include "corange_sound.h"
#include "corange_music.h"
#include "corange_lang.h"
#include "corange_font.h"
#include "corange_shader.h"
#include "corange_texture.h"
#include "corange_material.h"
#include "corange_renderable.h"
#include "corange_cmesh.h"
#include "corange_skeleton.h"
#include "corange_animation.h"
#include "corange_terrain.h"
#include "corange_effect.h"

/* UI */

#include "corange_cui.h"

#include "corange_ui_style.h"
#include "corange_ui_text.h"
#include "corange_ui_rectangle.h"
#include "corange_ui_spinner.h"
#include "corange_ui_button.h"
#include "corange_ui_textbox.h"
#include "corange_ui_browser.h"
#include "corange_ui_toast.h"
#include "corange_ui_dialog.h"
#include "corange_ui_listbox.h"
#include "corange_ui_option.h"
#include "corange_ui_slider.h"

/* Rendering */

#include "corange_sky.h"
#include "corange_renderer.h"

/* Data Structures */

#include "corange_dict.h"
#include "corange_list.h"
#include "corange_int_list.h"
#include "corange_vertex_list.h"
#include "corange_vertex_hashtable.h"
#include "corange_spline.h"
#include "corange_randf.h"

#endif
