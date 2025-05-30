#ifndef LUA_STACK_IMPL_H_
#define LUA_STACK_IMPL_H_
#include "mc_lua.hpp"
#include <unordered_map>
#include <filesystem>
#include <memory>
#include "mc_command.h"
#include <forward_list>
namespace lua
{
typedef std::unordered_map<std::string, event::Command> CommandMap;

class Guard;

class StackImpl final : public std::enable_shared_from_this<StackImpl>
{
public:
  StackImpl(std::filesystem::path const& path);
  ~StackImpl();
  auto Load(std::filesystem::path const& file) -> void;
  auto Call(int in, int out) -> void;
  auto Pop(int& out) -> void;
  auto Pop(float& out) -> void;
  auto Pop(double& out) -> void;
  auto Pop(std::string& out) -> void;
  auto Pop(bool& out) -> void;
  auto Pop(event::Command& out) -> void;
  auto Pop() -> void;
  auto Add(event::Command const& command, std::string const& name, int out, std::string const& library) -> void;
  auto Collect(int size) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Release() -> void;
  lua_State* state_;
  CommandMap map_;
  int weak_registry_;
  std::forward_list<Guard*> stack_;
};
}
#endif