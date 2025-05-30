#ifndef MENU_H_
#define MENU_H_
#include "mc_lua_stack.h"
#include "mc_window.h"
#include <memory>
#include <command.h>
#include <vector>
#include <filesystem>
namespace game
{
class Menu
{
public:
  Menu() = default;
  Menu(lua::Stack& lua, display::Window& window, std::filesystem::path const& path);
  auto Add(int index, event::Command const& command) -> void;
  auto Previous() -> void;
  auto Next() -> void;
  auto Select() -> void;
  auto Render() -> void;
  typedef std::pair<std::string, bool> Option;
  typedef std::vector<Option> Options;
  auto operator()(Options const& options) -> void;
  auto operator[](int index) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif