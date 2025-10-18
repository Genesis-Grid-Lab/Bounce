#include "Core/Factory.h"
#include "Core/FactoryDesc.h"
#include "Core/Types.h"
#include "Window/Window.h"
#include "Core/Log.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include "TestApp.h"

using namespace Engine;

int main(int argc, char** argv) {
  
    auto app = new TestApp();

    app->Run();

    delete app;

  return 0;  
}
