#ifndef BODY_IMPL_H_
#define BODY_IMPL_H_
#include <memory>
#include "b2Box2D.h"
#include "mc_world.h"
#include "mc_position.h"
#include "mc_body.h"
#include <array>
#include "mc_light.h"
#include "mc_modulation.h"
namespace dynamics
{
typedef std::array<b2Vec3, 4> Cubic;

class BodyImpl final : public std::enable_shared_from_this<BodyImpl>
{
public:
  BodyImpl(lua::Stack& lua, World& world);
  ~BodyImpl();
  auto Position() const -> game::Position;
  auto Position(float x, float y) -> void;
  auto Velocity() const -> game::Position;
  auto Velocity(float x, float y) -> void;
  auto Rotation() const -> double;
  auto Rotation(double angle) -> void;
  auto Force(float x, float y) -> void;
  auto Impulse(float x, float y) -> void;
  auto Begin() -> void;
  auto End(float32 dt) -> void;
  auto Update(float32 ds) -> void;
  auto Modulation() const -> display::Modulation;
  auto Emit(float r, float g, float b) -> void;
  auto Intrinsic(float r, float g, float b) -> void;
  static Body MakeBody(b2Body* body);
  World::WeakPtr world_;
  b2Body* body_;
  b2Vec3 position_;
  b2Vec3 velocity_;
  Cubic cubic_;
  Light light_;
  class Iterator;
};
}
#endif