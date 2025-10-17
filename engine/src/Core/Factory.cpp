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

namespace Engine {

FactoryDesc Factory::s_Desc = FactoryDesc{};

void Factory::SetDesc(const FactoryDesc& desc){
  s_Desc = desc;
}

Scope<IWindow> Factory::NewWindow() {
  switch(s_Desc.Window_API){
  case WindowAPI::GLFW:
    return CreateScope<GlfwWindow>(s_Desc);
  }

  E_CORE_ASSERT(false, "No window backend selected");
  return nullptr;
}
}
