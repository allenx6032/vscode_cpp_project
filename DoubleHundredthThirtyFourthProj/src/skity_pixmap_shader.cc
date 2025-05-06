#include "skity_pixmap_shader.hpp"

#include <skity_pixmap.hpp>

namespace skity {

PixmapShader::PixmapShader(std::shared_ptr<Pixmap> pixmap)
    : Shader(), pixmap_(std::move(pixmap)) {}

std::shared_ptr<Pixmap> PixmapShader::asImage() const { return pixmap_; }

}  // namespace skity