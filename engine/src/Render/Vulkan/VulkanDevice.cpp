#include "epch.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Window/Window.h"

namespace Engine {
  VulkanDevice::VulkanDevice() {}

  GraphicsAPI VulkanDevice::API() const { return GraphicsAPI::Vulkan; }

  bool VulkanDevice::Initialize(Window &window) { return true; }

  void VulkanDevice::BeginFrame(int fbWith, int fbHeight) {}

  void VulkanDevice::EndFrame() {
    
  }
}
