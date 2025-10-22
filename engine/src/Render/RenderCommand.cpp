#include "epch.h"
#include "Render/RenderCommand.h"
#include "Core/Factory.h"

namespace Engine {
  Scope<RendererAPI> RenderCommand::s_API = Factory::CreateRendererAPI();
}
