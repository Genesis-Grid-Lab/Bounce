#include "Render/RenderCommand.h"
#include "epch.h"
#include "Render/Renderer.h"
#include "Render/Renderer3D.h"

namespace Engine {

void Renderer::Init() { 
  RenderCommand::Init();
  Renderer3D::Init();
}

void Renderer::Shutdown() {
    Renderer3D::Shutdown();
  }

  void Renderer::OnWindowResize(uint32_t width, uint32_t height)
  {
    glm::vec4 viewport = {0, 0, width, height};
    RenderCommand::SetViewport(viewport);
}
}
