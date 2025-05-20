#ifndef ACTOR_H_
#define ACTOR_H_
#include <memory>
#include <string>
#include "mc_window.h"
#include "mc_position.h"
#include "mc_weak_ptr.h"
#include "mc_queue.h"
#include "mc_collision_group.h"
#include "mc_world.h"
#include <filesystem>
#include "mc_lua_stack.h"
namespace game
{
class Actor
{
public:
  Actor() = default;
  Actor(lua::Stack& lua, display::Window& window, collision::Group& collision, event::Queue& queue, dynamics::World& world, std::filesystem::path const& path);
  auto Position(game::Position const& position) -> void;
  auto Position() const -> game::Position;
  auto Velocity(game::Position const& velocity) -> void;
  auto Velocity() const -> game::Position;
  auto Force(game::Position const& force) -> void;
  auto Impulse(game::Position const& impulse) -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Body(std::string const& expression, int direction) -> void;
  auto Body(std::string const& expression) -> void;
  auto Body(int direction) -> void;
  auto Eyes(std::string const& expression) -> void;
  auto Mouth(std::string const& expression) -> void;
  auto Mouth(int open) -> void;
  auto Dilation() const -> double;
  auto Dilation(double dilation) -> void;
  auto Rotation(double angle) -> void;
  auto Rotation() const -> double;
  auto Scale(float scale) -> void;
  auto Scale() const -> float;
  auto Render() const -> void;
  auto Link(std::string const& group) -> void;
  auto Unlink(std::string const& group) -> void;
  auto Emit(float r, float g, float b) -> void;
  auto Intrinsic(float r, float g, float b) -> void;
  explicit operator bool() const;
  bool operator==(Actor const& other) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Actor> WeakPtr;
  friend WeakPtr;
  friend class JointImpl;
  static size_t Hash(Actor const& actor);
};
}

namespace std 
{
template<> struct hash<game::Actor> 
{
  size_t operator()(game::Actor const& actor) const 
  { 
    return game::Actor::Hash(actor); 
  }
};
}

// namespace boost 
// {
// template<> struct hash<game::Actor> 
// {
//   size_t operator()(game::Actor const& actor) const 
//   { 
//     return game::Actor::Hash(actor); 
//   }
// };
// }
#endif