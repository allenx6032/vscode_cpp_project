#ifndef ANIMATION_H_
#define ANIMATION_H_
#include "mc_window.h"
#include "mc_lua_stack.h"
#include <vector>
#include <filesystem>
namespace display
{
typedef std::vector<Texture> Animation;
auto MakeAnimation(lua::Stack& lua, Window& window, std::filesystem::path const& path) -> Animation;
}
#endif