#pragma once
#include "FactoryDesc.h"
#include "Window/IWindow.h"

namespace Engine {

class Factory {
public:
  static void SetDesc(const FactoryDesc &desc);
  static void SetWindowAPI(WindowAPI api);
  static WindowAPI GetWindowAPI() { return s_Desc.Window_API; }

  static Scope<IWindow> CreateWindow();
private:
  static FactoryDesc s_Desc;
};
}
