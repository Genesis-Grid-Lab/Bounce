#include "epch.h"
#include "Render/RenderCommand.h"

namespace Engine {

  Scope<RendererAPI> RenderCommand::s_API;

  void RenderCommand::Init(Scope<RendererAPI> api){
    s_API = std::move(api);
    if(s_API) s_API->Init();
  }

  void RenderCommand::Shutdown() { s_API.reset(); }

  void RenderCommand::SetViewport(glm::vec4& view) { s_API->SetViewport(view); }
  void RenderCommand::SetClearColor(glm::vec4& color){ s_API->SetClearColor(color); }
  void RenderCommand::Clear(){ s_API->Clear(); }
  void RenderCommand::EnableBlend(bool e){ s_API->EnableBlend(e); }
  void RenderCommand::DrawIndexed(uint32_t n){ s_API->DrawIndexed(n); }

}
