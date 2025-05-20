#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <memory>
#include "mc_bounding_box.h"
#include <string>
#include "mc_font.h"
#include "mc_shape.h"
#include "mc_modulation.h"
#include <filesystem>
namespace display
{
class Texture
{
public:
  Texture() = default;
  Texture(std::filesystem::path const& file, class Window& window, BoundingBox const& clip);
  Texture(std::string const& text, sdl::Font const& font, float width, class Window& window, BoundingBox const& clip);
  Texture(std::string const& text, sdl::Font const& font, class Window& window, BoundingBox const& clip);
  auto operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool;
  explicit operator bool() const;
  auto Shape() const -> Shape;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif