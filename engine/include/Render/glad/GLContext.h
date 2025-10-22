#pragma once

#include "Core/Types.h"
#include "Render/GraphicsContext.h"

struct GLFWwindow;

namespace Engine {

  class GLContext : public GraphicsContext{
  public:
    GLContext(GLFWwindow *windowHandle);

    void Init() override;
    void SwapBuffers() override;
  private:
    GLFWwindow* m_WindowHandle;
  };
}
