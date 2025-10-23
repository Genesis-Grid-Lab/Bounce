#pragma once
#include "Core/Layer.h"
#include "Core/Log.h"
#include "Render/Model.h"
#include "Render/Renderer3D.h"
#include "Render/Camera.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

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
  }
  virtual void OnDetach() override { }
  virtual void OnUpdate(Timestep ts) override {

    scene->OnUpdate(ts);
        
  }
  virtual void OnImGuiRender() override {}
private:
  Engine::Ref<Engine::Model> model;
  Engine::Ref<Engine::Scene> scene;
};
