#ifndef SKITY_SRC_RENDER_HW_VK_VK_FONT_TEXTURE_HPP
#define SKITY_SRC_RENDER_HW_VK_VK_FONT_TEXTURE_HPP

#include "skity_hw_font_texture.hpp"
#include "skity_vk_texture.hpp"

namespace skity {

class VKFontTexture : public HWFontTexture, public VKTexture {
 public:
  VKFontTexture(VKInterface* interface, Typeface* typeface,
                VKMemoryAllocator* allocator, VkRenderer* renderer,
                GPUVkContext* ctx);
  ~VKFontTexture() override;

  void Init() override;

  void Destroy() override;

  HWTexture* GetHWTexture() override;

 protected:
  void OnUploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                      uint8_t* data) override;

  void OnResize(uint32_t new_width, uint32_t new_height) override;
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_VK_VK_FONT_TEXTURE_HPP