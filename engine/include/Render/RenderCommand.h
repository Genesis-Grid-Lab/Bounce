#pragma once
#include "Core/Types.h"
#include "RendererAPI.h"

namespace Engine {

class RenderCommand {
public:
  static void Init(Scope<RendererAPI> api);
  static void Shutdown();

  static void SetViewport(glm::vec4& view);
  static void SetClearColor(glm::vec4& color);
  static void Clear();

  static void EnableBlend(bool e);

  static void DrawIndexed(uint32_t indexCount);
private:
  static Scope<RendererAPI> s_API;
};
}
