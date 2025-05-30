#ifndef INPUT_H_
#define INPUT_H_
#include "mc_lua_stack.h"
#include "mc_weak_ptr.h"
#include "mc_command.h"
namespace event
{
class Input
{
public:
  Input() = default;
  Input(lua::Stack& lua);
  void operator()();
  typedef std::function<bool(float, float)> Command;
  auto Control(Command const& command) -> void;
  auto Look(Command const& command) -> void;
  auto ChoiceUp(event::Command const& command) -> void;
  auto ChoiceDown(event::Command const& command) -> void;
  auto ChoiceLeft(event::Command const& command) -> void;
  auto ChoiceRight(event::Command const& command) -> void;
  auto ActionLeft(event::Command const& command) -> void;
  auto ActionRight(event::Command const& command) -> void;
  auto Select(event::Command const& command) -> void;
  auto Back(event::Command const& command) -> void;
  auto Quit(event::Command const& command) -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Input> WeakPtr;
  friend WeakPtr;
};
}
#endif