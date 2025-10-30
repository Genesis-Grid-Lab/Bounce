#pragma once

#include "Core/E_Assert.h"

namespace Engine {

  class PhysicsEngine{
  public:
    ~PhysicsEngine() = default;

    virtual void Init() = 0;

    virtual void StartSimulation() = 0;
    virtual bool Step(float dt) = 0;
    virtual void Shutdown() = 0;
  };
}
