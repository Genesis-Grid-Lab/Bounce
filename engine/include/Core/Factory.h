#pragma once
#include "FactoryDesc.h"
#include "Window/Window.h"

namespace Engine {

class Factory {
public:
  static void SetDesc(const FactoryDesc &desc);
  static WindowAPI GetWindowAPI() { return s_Desc.Window_API; }

  static Scope<Window> NewWindow();
private:
  static FactoryDesc s_Desc;
};
}
