#pragma once
#include "FactoryDesc.h"
#include "Render/GraphicsDevice.h"
#include "Window/Window.h"

namespace Engine {

class Factory {
public:
  static void SetDesc(const FactoryDesc &desc);
  static WindowAPI GetWindowAPI() { return s_Desc.Window_API; }

  static Scope<Window> NewWindow();
  static Scope<GraphicsDevice> NewGraphicsDevice();
  
private:
  static FactoryDesc s_Desc;
};
}
