#pragma once
#include "Core/Types.h"
#include "RendererAPI.h"

namespace Engine {

class RenderCommand {
public:
  static void Init() {
    s_API->Init();
  }

  static void SetViewport(glm::vec4 &view) {
    s_API->SetViewport(view);
  }
  static void SetClearColor(glm::vec4& color){
    s_API->SetClearColor(color);
  }
  static void Clear() {
    s_API->Clear();
  }

  static void EnableBlend(bool e) {
    s_API->EnableBlend(e);
  }

  static void DrawIndexed(const Ref<VertexArray>& vao,uint32_t indexCount){
    s_API->DrawIndexed(vao, indexCount);
  }
private:
  static Scope<RendererAPI> s_API;
};
}
