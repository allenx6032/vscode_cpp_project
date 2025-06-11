#include <lua.hpp>
#include "luasfml_wrap_render_window.h"
#include "luasfml_wrap_drawable.h"
#include "luasfml_wrap_sprite.h"
#include "luasfml_wrap_texture.h"
#include "luasfml_wrap_color.h"
#include "luasfml_wrap_float_rect.h"
#include "luasfml_wrap_int_rect.h"
#include "luasfml_wrap_font.h"
#include "luasfml_wrap_text.h"
#include "luasfml_wrap_circle_shape.h"
#include "luasfml_wrap_rectangle_shape.h"

void registerGraphicsModule(lua_State * l)
{
   register_TextEnums(l);

   register_sfRenderWindow(l);
   register_sfDrawable(l);
   register_sfSprite(l);
   register_sfTexture(l);
   register_sfColor(l);
   register_sfFloatRect(l);
   register_sfIntRect(l);
   register_sfFont(l);
   register_sfText(l);
   register_sfCircleShape(l);
   register_sfRectangleShape(l);
}

