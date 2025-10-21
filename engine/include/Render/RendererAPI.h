#pragma once
#include "Core/Types.h"

namespace Engine {

class RendererAPI {
public:
  virtual ~RendererAPI() = default;
  
  virtual void Init() = 0;

  virtual void SetViewport(glm::vec4& view) = 0;

  virtual void SetClearColor(glm::vec4& color) = 0;
  virtual void Clear() = 0;

  virtual void EnableBlend(bool enable) = 0;

  virtual void DrawIndexed(const class VertexArray& vao,uint32_t indexCount) = 0;
};
}
