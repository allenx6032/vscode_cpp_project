#ifndef GAME_BODY_H_
#define GAME_BODY_H_
#include <memory>
#include "mc_window.h"
#include "mc_lua_stack.h"
#include "mc_position.h"
#include <filesystem>
#include "mc_modulation.h"
#include "mc_bounding_box.h"
#include "mc_feature.h"
namespace game
{
class Body
{
public:
  Body() = default;
  Body(lua::Stack& lua, display::Window& window, std::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  auto Expression(std::string const& expression, int direction) -> double;
  auto Expression(std::string const& expression) -> double;
  auto Expression(int direction) -> double;
  auto Next() -> double;
  auto Period() const -> double;
  auto Position(Position const& position) -> void;
  auto Position() const -> game::Position;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Modulation() const -> display::Modulation;
  auto Render() const -> void;
  auto Position(game::Position const& position, double angle) -> void;
  auto Rotation(double angle) -> void;
  auto Rotation() const -> double;
  auto Scale(float scale) -> void;
  auto Scale() const -> float;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif