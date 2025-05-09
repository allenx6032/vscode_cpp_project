#include "skity_vk_interface.hpp"

namespace skity {

#define GET_PROC(F) \
  vk_interface->f##F = (decltype(vk_interface->f##F))proc_loader(device, "" #F)

VKInterface* VKInterface::InitInterface(VkDevice device,
                                        PFN_vkGetDeviceProcAddr proc_loader) {
  auto vk_interface = new VKInterface;

  GET_PROC(vkAllocateCommandBuffers);
  GET_PROC(vkAllocateDescriptorSets);
  GET_PROC(vkBeginCommandBuffer);
  GET_PROC(vkCmdBeginRenderPass);
  GET_PROC(vkCmdBindDescriptorSets);
  GET_PROC(vkCmdBindIndexBuffer);
  GET_PROC(vkCmdBindPipeline);
  GET_PROC(vkCmdBindVertexBuffers);
  GET_PROC(vkCmdCopyBufferToImage);
  GET_PROC(vkCmdDispatch);
  GET_PROC(vkCmdDrawIndexed);
  GET_PROC(vkCmdEndRenderPass);
  GET_PROC(vkCmdPipelineBarrier);
  GET_PROC(vkCmdPushConstants);
  GET_PROC(vkCmdSetScissor);
  GET_PROC(vkCmdSetStencilReference);
  GET_PROC(vkCmdSetStencilCompareMask);
  GET_PROC(vkCmdSetStencilWriteMask);
  GET_PROC(vkCmdSetViewport);
  GET_PROC(vkCreateCommandPool);
  GET_PROC(vkCreateComputePipelines);
  GET_PROC(vkCreateDescriptorPool);
  GET_PROC(vkCreateDescriptorSetLayout);
  GET_PROC(vkCreateFence);
  GET_PROC(vkCreateFramebuffer);
  GET_PROC(vkCreateGraphicsPipelines);
  GET_PROC(vkCreateImageView);
  GET_PROC(vkCreatePipelineLayout);
  GET_PROC(vkCreateRenderPass);
  GET_PROC(vkCreateSampler);
  GET_PROC(vkCreateShaderModule);
  GET_PROC(vkDestroyCommandPool);
  GET_PROC(vkDestroyDescriptorPool);
  GET_PROC(vkDestroyDescriptorSetLayout);
  GET_PROC(vkDestroyFence);
  GET_PROC(vkDestroyFramebuffer);
  GET_PROC(vkDestroyImageView);
  GET_PROC(vkDestroyPipeline);
  GET_PROC(vkDestroyPipelineLayout);
  GET_PROC(vkDestroyRenderPass);
  GET_PROC(vkDestroySampler);
  GET_PROC(vkDestroyShaderModule);
  GET_PROC(vkEndCommandBuffer);
  GET_PROC(vkGetPhysicalDeviceFeatures);
  GET_PROC(vkQueueSubmit);
  GET_PROC(vkQueueWaitIdle);
  GET_PROC(vkResetCommandPool);
  GET_PROC(vkResetDescriptorPool);
  GET_PROC(vkResetFences);
  GET_PROC(vkUpdateDescriptorSets);
  GET_PROC(vkWaitForFences);

  return vk_interface;
}

}  // namespace skity