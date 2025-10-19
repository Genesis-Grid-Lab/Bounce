#pragma once

#include "Core/FactoryDesc.h"

namespace Engine {
class Window;

class GraphicsDevice {
public:
  virtual ~GraphicsDevice() = default;
  virtual GraphicsAPI API() const = 0;
  virtual bool Initialize(Window& window) = 0;
  virtual void BeginFrame(int fbWidth, int fbHeight) = 0;
  virtual void EndFrame() = 0; 
};
}
