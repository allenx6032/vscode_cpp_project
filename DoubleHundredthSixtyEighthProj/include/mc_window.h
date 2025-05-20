#ifndef WINDOW_H_
#define WINDOW_H_
#include <string>
#include <memory>
#include "mc_font.h"
#include "mc_bounding_box.h"
#include "mc_lua_stack.h"
#include "mc_texture.h"
#include "mc_weak_ptr.h"
namespace display
{
class Window
{
public:
  Window() = default;
  Window(lua::Stack& lua);
  auto Clear() const -> void;
  auto Show() const -> void;
  auto View(float x, float y, float zoom) -> void;
  auto Rotation(double angle) -> void;
  auto Draw(BoundingBox const& box, Modulation const& modulation) const -> void;
  auto Scale() const -> float;
  typedef memory::WeakPtr<Window, class WindowImpl> WeakPtr;
  explicit operator bool() const;
private:
  std::shared_ptr<class WindowImpl> impl_;
  friend WeakPtr;
  friend class Texture::Impl;
};
}
#endif