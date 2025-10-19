#pragma once

#include "Render/GraphicsDevice.h"

namespace Engine {

  class VulkanDevice : public GraphicsDevice{
  public:
    VulkanDevice();
    GraphicsAPI API() const override;
    bool Initialize(Window &window) override;
    void BeginFrame(int fbWith, int fbHeight) override;
    void EndFrame() override;
  };
}
