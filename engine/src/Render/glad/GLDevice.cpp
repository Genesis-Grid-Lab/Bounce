#include "Render/glad/GLDevice.h"
#include "Window/Window.h"
#include "Render/RenderCommand.h"
#include "Core/Factory.h"
#include "Core/Log.h"
#include "Render/ViewportUtil.h"


// GLAD
#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Engine {

    GLDevice::GLDevice(){
  
}

bool GLDevice::Initialize(Window &window){
  m_Window = &window;

  // Ask window to ensure a GL context exist & is current (GLFW backend
  // implements this).
//   if(!m_Window->EnsureGLContext(/*major*/3, /*minor*/3, /*debug*/false)){
//     E_CORE_ERROR("Failed to ensure OpenGL context during GLDevice initialization.");
//     return false;
//   }

  glfwMakeContextCurrent((GLFWwindow*)m_Window->Handle());

  // Load GL via GLAD using window's proc loader
  auto loader = m_Window->GetGLProcLoader();
  if (!loader) {
    E_CORE_ERROR("No OpenGL procedure loader provided by the window backend.");
    return false;
  }

  if (!gladLoadGLLoader((GLADloadproc)loader)) {
    E_CORE_ERROR("gladLoadGLLoader failed to initialize OpenGL function pointers.");
    return false;
  }

  E_CORE_INFO("[OpenGL] Vendor:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
  E_CORE_INFO("[OpenGL] Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
  E_CORE_INFO("[OpenGL] Version:  {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

  // Basic GL state
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // Enable/disable vsync on the active context
  m_Window->SetSwapInterval(m_Window->IsVsyncEnabled() ? 1 : 0);

  m_Initialized = true;
  return true;
}

void GLDevice::BeginFrame(int fbWidth, int fbHeight) {
  if (!m_Initialized) return;

  // Get logical size (your IWindow already exposes it)
//   auto [lw, lh] = m_Window->GetLogicalSize();
//   if (lw <= 0 || lh <= 0) { lw = fbWidth; lh = fbHeight; }

//   // Integer scale that fits (pixel-perfect, centered)
//   const int sW = fbWidth  / lw;
//   const int sH = fbHeight / lh;
//   const int S  = std::max(1, std::min(sW, sH));

//   const int vpW = lw * S;
//   const int vpH = lh * S;
//   const int vpX = (fbWidth  - vpW) / 2;
//   const int vpY = (fbHeight - vpH) / 2;

//   RenderCommand::SetViewport(vpX, vpY, vpW, vpH);
}

void GLDevice::EndFrame() {
    if (!m_Initialized) return;
    m_Window->SwapBuffers(); // GLFW path swaps; others may be no-op or custom
}
}