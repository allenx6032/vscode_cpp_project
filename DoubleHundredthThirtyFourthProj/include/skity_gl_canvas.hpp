#ifndef SKITY_SRC_RENDER_HW_GL_GL_CANVAS_HPP
#define SKITY_SRC_RENDER_HW_GL_GL_CANVAS_HPP

#include <memory>

#include "skity_gl_renderer.hpp"
#include "skity_hw_canvas.hpp"

namespace skity {

class GLCanvas : public HWCanvas {
 public:
  GLCanvas(Matrix mvp, uint32_t width, uint32_t height, float density);
  ~GLCanvas() override = default;

 protected:
  void OnInit(GPUContext* ctx) override;
  bool SupportGeometryShader() override;
  std::unique_ptr<HWRenderer> CreateRenderer() override;
  std::unique_ptr<HWTexture> GenerateTexture() override;
  std::unique_ptr<HWFontTexture> GenerateFontTexture(
      Typeface* typeface) override;
  std::unique_ptr<HWRenderTarget> GenerateBackendRenderTarget(
      uint32_t width, uint32_t height) override;

 private:
  GPUContext* ctx_;
  GLRenderer* gl_renderer_;
  int gl_major_ = 0;
  int gl_minor_ = 0;
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_GL_GL_CANVAS_HPP