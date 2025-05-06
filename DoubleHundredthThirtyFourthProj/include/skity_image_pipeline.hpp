#ifndef SKITY_SRC_RENDER_HW_VK_PIPELINES_IMAGE_PIPELINE_HPP
#define SKITY_SRC_RENDER_HW_VK_PIPELINES_IMAGE_PIPELINE_HPP

#include "skity_static_pipeline.hpp"

namespace skity {

class StaticImagePipeline : public StaticPipeline {
 public:
  StaticImagePipeline(bool use_gs, size_t push_const_size)
      : StaticPipeline(use_gs, push_const_size) {}
  ~StaticImagePipeline() override = default;

  void UploadGradientInfo(GradientInfo const& info, GPUVkContext* ctx,
                          SKVkFrameBufferData* frame_buffer,
                          VKMemoryAllocator* allocator) override;

  void UploadImageTexture(VKTexture* texture, GPUVkContext* ctx,
                          SKVkFrameBufferData* frame_buffer,
                          VKMemoryAllocator* allocator) override;

 protected:
  VkDescriptorSetLayout GenerateColorSetLayout(GPUVkContext* ctx) override;

 private:
  glm::vec4 bounds_info_ = {};
};

class StencilDiscardImagePipeline : public StaticImagePipeline {
 public:
  StencilDiscardImagePipeline(bool use_gs, size_t push_const_size)
      : StaticImagePipeline(use_gs, push_const_size) {}
  ~StencilDiscardImagePipeline() override = default;

 protected:
  VkPipelineDepthStencilStateCreateInfo GetDepthStencilStateCreateInfo()
      override;
};

class StencilClipImagePipeline : public StaticImagePipeline {
 public:
  StencilClipImagePipeline(bool use_gs, size_t push_const_size)
      : StaticImagePipeline(use_gs, push_const_size) {}
  ~StencilClipImagePipeline() override = default;

  void UpdateStencilInfo(uint32_t reference, uint32_t compare_mask,
                         uint32_t write_mask, GPUVkContext* ctx) override;

 protected:
  std::vector<VkDynamicState> GetDynamicStates() override;
  VkPipelineDepthStencilStateCreateInfo GetDepthStencilStateCreateInfo()
      override;
};

class StencilKeepImagePipeline : public StaticImagePipeline {
 public:
  StencilKeepImagePipeline(bool use_gs, size_t push_const_size)
      : StaticImagePipeline(use_gs, push_const_size) {}
  ~StencilKeepImagePipeline() override = default;

 protected:
  VkPipelineDepthStencilStateCreateInfo GetDepthStencilStateCreateInfo()
      override;
};

class ImagePipelineFamily : public RenderPipelineFamily {
 public:
  ImagePipelineFamily() = default;
  ~ImagePipelineFamily() override = default;

 protected:
  std::tuple<const char*, size_t> GetFragmentShaderInfo() override;

  std::unique_ptr<AbsPipelineWrapper> CreateStaticPipeline(
      GPUVkContext* ctx) override;
  std::unique_ptr<AbsPipelineWrapper> CreateStencilDiscardPipeline(
      GPUVkContext* ctx) override;
  std::unique_ptr<AbsPipelineWrapper> CreateStencilClipPipeline(
      GPUVkContext* ctx) override;
  std::unique_ptr<AbsPipelineWrapper> CreateStencilKeepPipeline(
      GPUVkContext* ctx) override;
  std::unique_ptr<AbsPipelineWrapper> CreateOSStaticPipeline(
      GPUVkContext* ctx) override;
  std::unique_ptr<AbsPipelineWrapper> CreateOSStencilPipeline(
      GPUVkContext* ctx) override;
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_VK_PIPELINES_IMAGE_PIPELINE_HPP