#ifndef SKITY_SRC_RENDER_HW_GL_GL_PIPELINE_HPP
#define SKITY_SRC_RENDER_HW_GL_GL_PIPELINE_HPP

#include <array>
#include <memory>
#include <skity_gpu_context.hpp>

#include "skity_gl_shader.hpp"
#include "skity_hw_renderer.hpp"

namespace skity {

class GLRenderer : public HWRenderer {
 public:
  GLRenderer(GPUContext* ctx, bool use_gs);
  ~GLRenderer() override;

  void Init() override;

  void Destroy() override;

  void Bind() override;

  void UnBind() override;

  void SetViewProjectionMatrix(glm::mat4 const& mvp) override;

  void SetModelMatrix(glm::mat4 const& matrix) override;

  void SetPipelineColorMode(HWPipelineColorMode mode) override;

  void SetStrokeWidth(float width) override;

  void SetUniformColor(glm::vec4 const& color) override;

  void SetGradientBoundInfo(glm::vec4 const& info) override;

  void SetGradientCountInfo(int32_t color_count, int32_t pos_count) override;

  void SetGradientColors(std::vector<Color4f> const& colors) override;

  void SetGradientPositions(std::vector<float> const& pos) override;

  void UploadVertexBuffer(void* data, size_t data_size) override;

  void UploadIndexBuffer(void* data, size_t data_size) override;

  void SetGlobalAlpha(float alpha) override;

  void EnableStencilTest() override;

  void DisableStencilTest() override;

  void EnableColorOutput() override;

  void DisableColorOutput() override;

  void UpdateStencilMask(uint8_t write_mask) override;

  void UpdateStencilOp(HWStencilOp op) override;

  void UpdateStencilFunc(HWStencilFunc func, uint32_t value,
                         uint32_t compare_mask) override;

  void DrawIndex(uint32_t start, uint32_t count) override;

  void BindTexture(HWTexture* texture, uint32_t slot) override;

  void BindRenderTarget(HWRenderTarget* render_target) override;

  void UnBindRenderTarget(HWRenderTarget* render_target) override;

 private:
  void InitShader();
  void InitBufferObject();

  void BindBuffers();
  void UnBindBuffers();

 private:
  GPUContext* ctx_;
  bool use_gs_;
  std::unique_ptr<GLPipelineShader> shader_ = {};
  uint32_t vao_ = 0;
  std::array<uint32_t, 2> buffers_ = {};
  std::array<size_t, 2> buffer_sizes_ = {};
  glm::ivec4 saved_viewport_ = {};
  int32_t root_fbo_ = 0;
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_GL_GL_PIPELINE_HPP