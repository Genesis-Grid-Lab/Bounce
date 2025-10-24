#pragma once
#include "Core/Layer.h"
#include "Core/Log.h"
#include "Render/Model.h"
#include "Render/Renderer3D.h"
#include "Render/Camera.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Core/Input.h"

class TestLayer : public Engine::Layer {
public:
  TestLayer() {}

  void OnAttach() override {
    E_INFO("OnAttach()");
    scene = Engine::CreateRef<Engine::Scene>();
    model = Engine::CreateRef<Engine::Model>(
        "Resources/sponza2/source/glTF/Sponza.gltf");

    auto MainModel = scene->CreateEntity("sponza");
    MainModel.AddComponent<Engine::ModelComponent>().ModelData = model;

    auto cam = scene->CreateEntity("cam");
    auto& cc = cam.AddComponent<Engine::Camera3DComponent>();
    cc.Primary = true;
    auto& tc = cam.GetComponent<Engine::TransformComponent>().Translation;
    tc = {0, 0, 2};
  }
  virtual void OnDetach() override { }
  virtual void OnUpdate(Timestep ts) override {

    scene->OnUpdate(ts);
    if(Engine::Input::IsKeyPressed(Key::Space)){
        scene->TogglePlaying();
    }
  }
  virtual void OnImGuiRender() override {}
private:
  Engine::Ref<Engine::Model> model;
  Engine::Ref<Engine::Scene> scene;
};
