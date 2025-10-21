#pragma once

#include "Render/RendererAPI.h"

namespace Engine {

class VulkanRendererAPI : public RendererAPI {
public:
  void Init() override;
  void SetViewport(glm::vec4 &view) override;
  void SetClearColor(glm::vec4 &color) override;
  void Clear() override;

  void EnableBlend(bool enable) override;

  void DrawIndexed(const class VertexArray& vao,uint32_t indexCount) override;
};
}
