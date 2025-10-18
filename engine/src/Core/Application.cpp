#include "epch.h"
#include "Core/Application.h"
#include "Core/E_Assert.h"
#include "Core/Factory.h"
#include "Core/Timestep.h"

// temp
#include <GLFW/glfw3.h>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef E_DEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    if (!layerFound) {
        return false;
    }
    }

    return true;
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

namespace Engine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, const glm::vec2& size){
        Log::Init();
        E_CORE_ASSERT(!s_Instance, "Application already exists!");
        E_CORE_INFO("[APPLICATION] Starting");
        FactoryDesc desc;
        desc.Title = name;
        desc.DisplaySize = size;
        desc.Window_API = WindowAPI::GLFW;
        Factory::SetDesc(desc);
        s_Instance = this;
        m_Window = Factory::NewWindow();

        createInstance();
        setupDebugMessenger();
	pickPhysicalDevice();

        m_Window->Events().Subscribe<EWindowResize>([this](const EWindowResize& e) { OnWindowResize(e);});
        m_Window->Events().Subscribe<EWindowClose>([this](const EWindowClose&) { OnWindowClose();});        
    }

    Application::~Application(){
        E_CORE_INFO("[APPLICATION] Shutting down");
	if (enableValidationLayers) {
	  DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}        
	vkDestroyInstance(instance, nullptr);
    }

    void Application::PushLayer(Layer* layer)
    {    
        E_CORE_INFO("[APPLICATION] Pushing layer");
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        E_CORE_INFO("[APPLICATION] Pushing overlay");
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer) {
        E_CORE_INFO("[APPLICATION] Pop layer");
        m_LayerStack.PopLayer(layer);
    }

    void Application::Close(){
        E_CORE_INFO("[APPLICATION] Closing");
        m_Running = false;
    }

    void Application::Run(){
        while(m_Running && !m_Window->ShouldClose()){
            // float time = (float)
            Timestep timestep = m_LastFrameTime;
            // m_LastFrameTime = time;

            if(!m_Minimized){
                for(Layer* layer : m_LayerStack){
                    layer->OnUpdate(timestep);
                }
            }
            m_Window->PollEvents();
            m_Window->SwapBuffers();

            while (!m_LayerActionQueue.empty()) {
                LayerAction action = m_LayerActionQueue.front();
                m_LayerActionQueue.pop();
            
                if (action.Type == LayerActionType::Push) {
                    PushLayer(action.LayerPtr);
                } else if (action.Type == LayerActionType::Pop) {
                    PopLayer(action.LayerPtr);
                    delete action.LayerPtr;
                }
            }
        }
    }


    void Application::OnWindowClose() {
      E_CORE_INFO("[APPLICATION] Closing");
        m_Running = false;        
    }
    void Application::OnWindowResize(const EWindowResize& e){
      E_CORE_INFO("[APPLICATION] Resizing");
      if(e.fbWidth == 0 || e.fbHeight == 0){
	m_Minimized = true;            
      }

      m_Minimized = false;
    }

    void Application::createInstance() {
      if (enableValidationLayers && !checkValidationLayerSupport()) {
        E_CORE_ASSERT(false, "validation layers requested, but not available!");
      }
      
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Hello triangle";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions;

      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
      auto extensions = getRequiredExtensions();
      createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();

      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
      if (enableValidationLayers) {
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

	populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
      } else {
        createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
      }

      VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

      if (result != VK_SUCCESS) {
	E_CORE_ASSERT(false, "failed to create vulkan instance");
      }

      uint32_t extensionCount = 0;
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
      std::vector<VkExtensionProperties> extensionss(extensionCount);
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                             extensionss.data());
      E_CORE_INFO("available extensions:");

      for (const auto& extension : extensionss) {
        E_CORE_INFO("{}" ,extension.extensionName);
      }
    }

    void Application::pickPhysicalDevice() {
      
    }

  void Application::setupDebugMessenger() {
      if (!enableValidationLayers)
        return;
      VkDebugUtilsMessengerCreateInfoEXT createInfo{};
      populateDebugMessengerCreateInfo(createInfo);
      createInfo.pUserData = nullptr; // Optional

      if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        E_CORE_ASSERT(false,"failed to set up debug messenger!");
      }
  }
}
