#ifndef TIMESLICE_H_
#define TIMESLICE_H_
#include "mc_command.h"
#include <memory>
#include "mc_weak_ptr.h"
#include "mc_lua_stack.h"
namespace event
{
class Timeslice
{
public:
  Timeslice() = default;
  Timeslice(lua::Stack& lua);
  auto Add(Command const& command) -> void;
  auto Resume() -> void;
  auto Pause() -> void;
  auto operator()() -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Timeslice, Impl> WeakPtr;
  friend WeakPtr;
};
}
#endif