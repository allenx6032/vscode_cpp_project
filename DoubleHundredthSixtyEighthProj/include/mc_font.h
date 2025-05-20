#ifndef FONT_H_
#define FONT_H_
#include <memory>
#include "mc_lua_stack.h"
#include <filesystem>
namespace sdl
{
class Font
{
public:
  Font() = default;
  Font(lua::Stack& lua, std::filesystem::path const& path);
  auto LineSpacing() const -> float;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
  friend class Surface;
};
}
#endif