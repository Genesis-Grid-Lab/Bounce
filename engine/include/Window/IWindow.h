#pragma once
#include "Core/Types.h"
#include "Core/FactoryDesc.h"
#include "Event/EventBus.h"

namespace Engine {

  class IWindow{
  public:
    using CloseCallback = std::function<void()>;
    using ResizeCallback =
      std::function<void(int fbWidth, int fbHeight, float dpr)>;
    virtual ~IWindow() = default;
    virtual WindowAPI API() = 0;
    virtual void* Handle() const = 0;
    
    virtual bool ShouldClose() const = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;

    virtual glm::vec2 GetWindowSize() const = 0;
    virtual glm::vec2 GetFramebufferSize() const = 0;
    virtual float GetDevicePixelRatio() const = 0;

    virtual void SetCloseCallback(CloseCallback cb) = 0;
    virtual void SetResizeCallback(ResizeCallback cb) = 0;

    virtual EventBus &Events() = 0;

    virtual void SetSwapInterval(int interval) { (void)interval; }
    virtual bool IsVsyncEnabled() const { return false; }    
  private:    
  };
}
