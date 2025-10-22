#pragma once

#include "Render/RendererAPI.h"

namespace Engine {

class VulkanRendererAPI : public RendererAPI {
public:
  void Init() override;
  void SetViewport(glm::vec4 &view) override;
  void SetClearColor(const glm::vec4 &color) override;
  void Clear() override;

  void DrawIndexed(const Ref<VertexArray>& vao,uint32_t indexCount) override;
};
}
