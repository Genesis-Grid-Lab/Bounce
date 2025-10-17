#pragma once

#include "Types.h"

namespace Engine {
  enum class WindowAPI { None, GLFW, Native, Android, IOS };

  struct FactoryDesc {
    std::string Title = "";

    glm::vec2 DisplaySize = {0,0};

    bool Resizeable = false;
    bool Fullscreen = false;

    bool VSync = false;

    // Desire API
    WindowAPI Window_API = WindowAPI::None;
  };

}
