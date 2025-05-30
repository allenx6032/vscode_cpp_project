#ifndef SIGNAL_H_
#define SIGNAL_H_
#include <memory>
#include "mc_command.h"
#include "mc_queue.h"
#include "mc_weak_ptr.h"
namespace event
{
class Signal
{
public:
  Signal();
  auto operator()() -> void;
  auto operator()(Queue& queue) -> void;
  auto Add(Command const& command) -> void;
  auto Clear() -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Signal> WeakPtr;
  friend WeakPtr;
};
}
#endif