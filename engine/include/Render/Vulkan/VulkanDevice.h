// #pragma once

// #include "Render/GraphicsDevice.h"
// #include <vulkan/vulkan.h>

// struct QueueFamilyIndices {
//   std::optional<uint32_t> graphicsFamily;
//   std::optional<uint32_t> presentFamily;

//   bool isComplete() {
//     return graphicsFamily.has_value() && presentFamily.has_value();
//   }
// };

// struct SwapChainSupportDetails {
//   VkSurfaceCapabilitiesKHR capabilities;
//   std::vector<VkSurfaceFormatKHR> formats;
//   std::vector<VkPresentModeKHR> presentModes;
// };

// namespace Engine {

//   class VulkanDevice : public GraphicsDevice{
//   public:
//     VulkanDevice();
//     ~VulkanDevice() override;
//     GraphicsAPI API() const override;
//     bool Initialize(Window &window) override;
//     void BeginFrame(int fbWith, int fbHeight) override;
//     void EndFrame() override;
//   private:
//     void createInstance();
//     void setupDebugMessenger();
//     void createSurface();
//     void pickPhysicalDevice();
//     void createLogicalDevice();
//     void createSwapChain();
//     void createImageViews();
//     void createRenderPass();
//     void createGraphicsPipeline();
//     void createFramebuffers();
//     void createCommandPool();
//     void createCommandBuffer();
//     void drawFrame();
//     void createSyncObjects();
    
//     VkInstance instance;
//     VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//     VkDevice device;
//     VkQueue graphicsQueue;
//     VkQueue presentQueue;
//     VkDebugUtilsMessengerEXT debugMessenger;
//     VkSurfaceKHR surface;
//     VkSwapchainKHR swapChain;
    
//     std::vector<VkImage> swapChainImages;
//     std::vector<VkImageView> swapChainImageViews;
//     std::vector<VkFramebuffer> swapChainFramebuffers;
    
//     VkFormat swapChainImageFormat;
//     VkExtent2D swapChainExtent;
//     VkRenderPass renderPass;
//     VkPipelineLayout pipelineLayout;
//     VkPipeline graphicsPipeline;
//     VkCommandPool commandPool;
//     VkCommandBuffer commandBuffer;

//     VkSemaphore imageAvailableSemaphore;
//     VkSemaphore renderFinishedSemaphore;
//     VkFence inFlightFence;
//     class Window* m_Window;

//    private:
//     QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
//     bool isDeviceSuitable(VkPhysicalDevice device);
//     int rateDeviceSuitability(VkPhysicalDevice device);
//     bool checkDeviceExtensionSupport(VkPhysicalDevice device);
//     void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
//     SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
//     VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
//     VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
//     VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
//     VkShaderModule createShaderModule(const std::vector<char> &code);
//   };
// }
