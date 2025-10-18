#pragma once
#include "Core/Layer.h"
#include "Core/Log.h"

class TestLayer : public Engine::Layer {
public:
    TestLayer() {}

    void OnAttach() override { E_INFO("OnAttach()"); }
    virtual void OnDetach() override { }
    virtual void OnUpdate(Timestep ts) override { }
    virtual void OnImGuiRender() override {}
};