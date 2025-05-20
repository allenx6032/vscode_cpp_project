#ifndef SUBTITLE_H_
#define SUBTITLE_H_
#include "mc_lua_stack.h"
#include "mc_window.h"
#include <memory>
#include <filesystem>
namespace game
{
class Subtitle
{
public:
  Subtitle() = default;
  Subtitle(lua::Stack& lua, display::Window& window, std::filesystem::path const& path);
  auto operator()(std::string const& text) -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Render() -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif