#include "Core/FactoryDesc.h"
#include "Render/GraphicsDevice.h"
#include "epch.h"
#include "Core/Factory.h"
#include "Core/E_Assert.h"

//temp
#include "Window/GLFW/GlfwWindow.h"
#if defined(WINDOW_BACKEND_GLFW)
  #include "Window/GLFW/GlfwWindow.h"
#elif defined(WINDOW_BACKEND_NATIVE)
  #include "Window/Native/NativeWindow.h"
#elif defined(WINDOW_BACKEND_ANDROID)
  #include "Window/Android/AndroidWindow.h"
#elif defined(WINDOW_BACKEND_IOS)
  #include "Window/IOS/IosWindow.h"
#endif

#include "Render/Vulkan/VulkanDevice.h"

namespace Engine {

FactoryDesc Factory::s_Desc = FactoryDesc{};

void Factory::SetDesc(const FactoryDesc& desc){
  s_Desc = desc;
}

Scope<Window> Factory::NewWindow() {
  switch(s_Desc.Window_API){
  case WindowAPI::GLFW:
    return CreateScope<GlfwWindow>(s_Desc);
  }

  E_CORE_ASSERT(false, "No window backend selected");
  return nullptr;
}

  Scope<GraphicsDevice> Factory::NewGraphicsDevice(){
    switch(s_Desc.Graphics_API){
    case GraphicsAPI::Vulkan:
      return CreateScope<VulkanDevice>();
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }
}
