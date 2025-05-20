#ifndef WORLD_H_
#define WORLD_H_
#include <memory>
#include "mc_lua_stack.h"
#include "mc_weak_ptr.h"
#include "mc_command.h"
#include "mc_collision_group.h"
#include "mc_queue.h"
namespace dynamics
{
class World
{
public:
  World() = default;
  World(lua::Stack& lua, collision::Group& collision, event::Queue& queue);
  explicit operator bool() const;
  auto Begin(event::Command const& command) -> void;
  auto End(event::Command const& command) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Ambient(float r, float g, float b) -> void;
  auto operator==(World const& other) const -> bool;
  typedef memory::WeakPtr<World, class WorldImpl> WeakPtr;
private:
  std::shared_ptr<class WorldImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
  friend class JointImpl;
};
}
#endif