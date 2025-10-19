#pragma once

#include "Core/Types.h"
#include "Window/Window.h"
#include "Render/GraphicsDevice.h"
#include "Core/Timestep.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"
// #include "Event/EventBus.h"

// temp
#include <vulkan/vulkan.h>

int main(int argc, char** argv);

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

namespace Engine {

  enum class LayerActionType {
    Push,
    Pop
  };

  struct LayerAction {
    LayerActionType Type;
    Layer* LayerPtr;
  };

  class Application {
  public:
    Application(const std::string& name = "Application", const glm::vec2& size = glm::vec2(100));
    virtual ~Application();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);
    void PopLayer(Layer* layer);

    void Close();

    Window& GetWindow() { return *m_Window;}
    static Application& Get() { return *s_Instance;}

    void QueueLayerAction(LayerActionType type, Layer* layer) {
      m_LayerActionQueue.push({ type, layer });
    }

  private:
    void Run();
    void OnWindowClose();
    void OnWindowResize(const EWindowResize& e);
  private:
    Scope<Window> m_Window;
    Scope<GraphicsDevice> m_GraphicsDevice;

    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
    std::queue<LayerAction> m_LayerActionQueue;        
  private:
    friend int ::main(int argc, char** argv);
    static Application *s_Instance;

    // temp
  private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffer();
    void drawFrame();
    void createSyncObjects();
    
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    
  private:
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    int rateDeviceSuitability(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkShaderModule createShaderModule(const std::vector<char> &code);
  };
}
