#include "Core/Factory.h"
#include "Core/FactoryDesc.h"
#include "Core/Types.h"
#include "Window/IWindow.h"
#include <iostream>

using namespace Engine;

int main() {
  FactoryDesc desc;
  desc.Title = "App";
  desc.DisplaySize = {200, 200};
  desc.Window_API = WindowAPI::GLFW;
  
  Factory::SetDesc(desc);
  Scope<IWindow> window = Factory::CreateWindow();

  while (!window->ShouldClose()) {
    window->PollEvents();
    window->SwapBuffers();
  }
  return 0;  
}
