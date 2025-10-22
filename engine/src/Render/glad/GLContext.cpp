#include "epch.h"
#include "Render/glad/GLContext.h"
#include "Core/E_Assert.h"


// GLAD
#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Engine {
  
  GLContext::GLContext(GLFWwindow* windowHandle): m_WindowHandle(windowHandle){
    E_CORE_ASSERT(windowHandle, "window handle is null!");
  }

  void GLContext::Init(){
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    E_CORE_ASSERT(status, "Failed to initialize Glad!");

    E_CORE_INFO("OpenGL Info:");
    E_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
    E_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
    E_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

    E_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Engine requires at least OpenGL version 4.5!");
  }

  void GLContext::SwapBuffers()
  {
    glfwSwapBuffers(m_WindowHandle);
  }
}
