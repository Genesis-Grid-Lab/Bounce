#pragma once

#include "Types.h"

namespace Engine {
  enum class WindowAPI { None, GLFW, Native, Android, IOS };
  enum class GraphicsAPI { None, OpenGL, Vulkan, DirectX, Metal};

  struct FactoryDesc {
    std::string Title = "";

    glm::vec2 DisplaySize = {0,0};

    bool Resizeable = false;
    bool Fullscreen = false;

    bool VSync = false;

    // Desire API
    WindowAPI Window_API = WindowAPI::None;
    GraphicsAPI Graphics_API = GraphicsAPI::None;
  };

}
