#ifndef SURFACE_H_
#define SURFACE_H_
#include "mc_font.h"
#include <string>
#include <memory>
#include "SDL_surface.h"
#include <filesystem>
namespace sdl
{
class Surface
{
public:
  Surface() = default;
  Surface(std::string const& text, Font const& font);
  Surface(std::string const& text, Font const& font, Uint32 length);
  Surface(std::filesystem::path const& file);
  explicit operator SDL_Surface*() const;
private:
  std::shared_ptr<SDL_Surface> impl_;
};
}
#endif