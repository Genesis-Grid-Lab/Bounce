#include "Core/FactoryDesc.h"
#include "Render/GraphicsContext.h"
#include "Render/glad/GLRendererAPI.h"
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
#include "Render/Vulkan/VulkanRendererAPI.h"

#include "Render/glad/GLContext.h"
#include "Render/glad/GLShader.h"
#include "Render/glad/GLVertexArray.h"
#include "Render/glad/GLBuffer.h"

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

  Scope<GraphicsContext> Factory::Create(void* window){
    switch(s_Desc.Graphics_API){
    case GraphicsAPI::Vulkan:
      // return CreateScope<VulkanDevice>();
    case GraphicsAPI::OpenGL:
      return CreateScope<GLContext>(static_cast<GLFWwindow*>(window));
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Scope<RendererAPI> Factory::CreateRendererAPI(){
    switch(s_Desc.Graphics_API){
    case GraphicsAPI::OpenGL:
      return CreateScope<GLRendererAPI>();
    case GraphicsAPI::Vulkan:
      return CreateScope<VulkanRendererAPI>();
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Ref<Shader> Factory::CreateShader(const std::string& filepath){
    switch(s_Desc.Graphics_API){
      case GraphicsAPI::OpenGL:
        return CreateRef<GLShader>(filepath);
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Ref<VertexBuffer> Factory::Create(uint32_t size){
    switch(s_Desc.Graphics_API){
      case GraphicsAPI::OpenGL:
        return CreateRef<GLVertexBuffer>(size);
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Ref<VertexBuffer> Factory::Create(float* vertices, uint32_t size){
    switch(s_Desc.Graphics_API){
      case GraphicsAPI::OpenGL:
        return CreateRef<GLVertexBuffer>(vertices,size);
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Ref<VertexBuffer> Factory::Create(Vertex *vertices, uint32_t size){
    switch(s_Desc.Graphics_API){
      case GraphicsAPI::OpenGL:
        return CreateRef<GLVertexBuffer>(vertices,size);
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }

  Ref<VertexArray> Factory::Create(){
    switch(s_Desc.Graphics_API){
      case GraphicsAPI::OpenGL:
        return CreateRef<GLVertexArray>();
    }

    E_CORE_ASSERT(false, "No graphics backend selected");
    return nullptr;
  }
}
