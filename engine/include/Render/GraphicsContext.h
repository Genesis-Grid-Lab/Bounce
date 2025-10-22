#pragma once

#include "Core/FactoryDesc.h"

namespace Engine {
class Window;

class GraphicsContext {
public:
  virtual ~GraphicsContext() = default;
  virtual void Init() = 0;

  virtual void SwapBuffers() = 0;
};
}
