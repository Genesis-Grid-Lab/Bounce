#include "epch.h"
#include "Core/Factory.h"

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

Scope<IWindow> Factory::CreateWindow() {
  switch(s_Desc.Window_API){
  case WindowAPI::GLFW:
    return CreateScope<GlfwWindow>(s_Desc);
  }

  // static_assert(false, "No window backend selected");
  return nullptr;
}
}
