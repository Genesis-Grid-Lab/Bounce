#pragma once

#include "Core/Types.h"
#include "Window/Window.h"
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
      void createGraphicsPipeline();
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
      VkFormat swapChainImageFormat;
      VkExtent2D swapChainExtent;
    private:
      QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
      bool isDeviceSuitable(VkPhysicalDevice device);
      int rateDeviceSuitability(VkPhysicalDevice device);
      bool checkDeviceExtensionSupport(VkPhysicalDevice device);
      SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
      VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
      VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
      VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}
