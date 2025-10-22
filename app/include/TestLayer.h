#pragma once
#include "Core/Layer.h"
#include "Core/Log.h"
#include "Render/Renderer3D.h"
#include "Render/Camera.h"

class TestLayer : public Engine::Layer {
public:
    TestLayer() {}

    void OnAttach() override { 
        E_INFO("OnAttach()");
        camera = Engine::Camera3D(30.0f, 1.778f, 0.1f, 2000.0f);
    }
    virtual void OnDetach() override { }
    virtual void OnUpdate(Timestep ts) override {
        Engine::Renderer3D::BeginCamera(camera);

        Engine::Renderer3D::DrawCube({0, 0, 0}, {1, 1, 1}, {10, 0, 0});

        Engine::Renderer3D::EndCamera();
     }
    virtual void OnImGuiRender() override {}
private:
 Engine::Camera3D camera;
};