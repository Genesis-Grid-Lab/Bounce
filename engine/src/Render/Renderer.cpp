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
}
