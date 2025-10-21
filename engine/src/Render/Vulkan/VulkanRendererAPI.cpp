#include "epch.h"
#include "Render/Vulkan/VulkanRendererAPI.h"

namespace Engine {

void VulkanRendererAPI::Init() {}

void VulkanRendererAPI::SetViewport(glm::vec4 &view) {}

void VulkanRendererAPI::SetClearColor(glm::vec4 &color) {}

void VulkanRendererAPI::Clear() {}

void VulkanRendererAPI::EnableBlend(bool enable) {}

void VulkanRendererAPI::DrawIndexed(const class VertexArray& vao,uint32_t indexCount) {}

}
