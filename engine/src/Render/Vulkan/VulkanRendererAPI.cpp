#include "epch.h"
#include "Render/Vulkan/VulkanRendererAPI.h"

namespace Engine {

void VulkanRendererAPI::Init() {}

void VulkanRendererAPI::SetViewport(glm::vec4 &view) {}

void VulkanRendererAPI::SetClearColor(const glm::vec4 &color) {}

void VulkanRendererAPI::Clear() {}

// void VulkanRendererAPI::EnableBlend(bool enable) {}

void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vao,uint32_t indexCount) {}

}
