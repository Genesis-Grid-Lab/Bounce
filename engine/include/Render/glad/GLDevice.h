#pragma once

#include "Core/Types.h"
#include "Render/GraphicsDevice.h"

namespace Engine {

    class GLDevice : public GraphicsDevice{
 public:
  explicit GLDevice();
  GraphicsAPI API() const override { return GraphicsAPI::OpenGL; }
  bool Initialize(Window& window) override;
  void BeginFrame(int fbWidth, int fbHeight) override;
  void EndFrame() override;

  void SetClearColor(glm::vec4 color) {
    color = m_Clear;
  }

 private:
  class Window* m_Window = nullptr;
  glm::vec4 m_Clear = {0.08f, 0.09f, 0.10f, 1.0f};
  bool m_Initialized = false;
};
}