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
#include "Core/ImGuiHelper.h"

class TestLayer : public Engine::Layer {
public:
  TestLayer() {}

  void OnAttach() override {
    E_INFO("OnAttach()");
    scene = Engine::CreateRef<Engine::Scene>();
    model = Engine::CreateRef<Engine::Model>(
        "Resources/sponza2/source/glTF/Sponza.gltf");

    Engine::Ref<Engine::Model> Man = Engine::CreateRef<Engine::Model>("Resources/Animations/Idle.fbx");
    Engine::Ref<Engine::Animation> ManAnim = CreateRef<Engine::Animation>("Resources/Animations/Idle.fbx", Man);

    auto MainModel = scene->CreateEntity("sponza");
    MainModel.AddComponent<Engine::ModelComponent>().ModelData = model;
    auto &modelT = MainModel.GetComponent<Engine::TransformComponent>();
    modelT.Scale = glm::vec3(0.2f);

    cam = scene->CreateEntity("cam");
    auto& cc = cam.AddComponent<Engine::Camera3DComponent>();
    cc.Primary = true;
    auto& tc = cam.GetComponent<Engine::TransformComponent>().Translation;
    tc = {0, 0, 2};

    manEntt = scene->CreateEntity("Man");
    auto& manTC = manEntt.GetComponent<Engine::TransformComponent>();
    manTC.Translation = {1, 2, 0};
    // manTC.Scale = glm::vec3(0.1f);
    manTC.Rotation = {0, glm::radians(180.0f), 0};
    auto& rgbc = manEntt.AddComponent<Engine::RigidbodyComponent>();
    rgbc.Type = Engine::BodyType::Dynamic;
    rgbc.Shape = new Engine::BoxShape({1.0f,1.0f,1.0f});
    auto& manModel = manEntt.AddComponent<Engine::ModelComponent>();
    manModel.ModelData = Man;
    manModel.AnimationData["idle"] = ManAnim;
    // manModel.AnimationData["run"] = runAnim;
    // manModel.AnimationData["jump"] = jumpAnim;
    

    cubeMod = scene->CreateEntity("cube");
    cubeMod.AddComponent<Engine::CubeComponent>().Color = {1, 0, 1};
    auto& cubeRbc = cubeMod.AddComponent<Engine::RigidbodyComponent>();
    cubeRbc.Type = Engine::BodyType::Static;
    cubeRbc.Shape = new Engine::BoxShape();
    auto &cubetc = cubeMod.GetComponent<Engine::TransformComponent>();
    cubetc.Translation = {1, 1, 0};

    auto sphereMod = scene->CreateEntity("sphere");
    sphereMod.AddComponent<Engine::SphereComponent>().Color = {0.5f, 0.1f, 0.7f};
    auto &spheretc = sphereMod.GetComponent<Engine::TransformComponent>();
    cubetc.Translation = {2, 1, 1};
  }
  virtual void OnDetach() override { }
  virtual void OnUpdate(Timestep ts) override {

    switch (m_SceneState) {
    case Engine::SceneState::Edit:
      scene->OnUpdate(ts);
      break;
    case Engine::SceneState::Play:
      scene->OnUpdateRuntime(ts);
      break;
    }

    // if (Engine::Input::IsKeyJustPressed(Key::Space)) {
    //   switch (m_SceneState) {
    //   case Engine::SceneState::Edit:
	// E_INFO("Change to play");
    //     m_SceneState = Engine::SceneState::Play;
	// break;
    //   case Engine::SceneState::Play:
	// E_INFO("Change to edit");
	// m_SceneState = Engine::SceneState::Edit;
	// break;
    //   }
    // }

    auto &cc = cam.GetComponent<Engine::Camera3DComponent>();
    cc.Camera.SetTarget(target);
  }
  virtual void OnImGuiRender() override {
    auto &tc = cam.GetComponent<Engine::TransformComponent>().Translation;
    auto &manTc = manEntt.GetComponent<Engine::TransformComponent>().Translation;
    auto& cubeModTc = cubeMod.GetComponent<Engine::TransformComponent>();
    ImGui::Begin("Properties");
    ImGui::Text("Camera");
    DrawVec3Control("CamMove", tc);
    DrawVec3Control("Target", target);
    ImGui::Separator();
    ImGui::Text("Man");
    DrawVec3Control("Man Pos", manTc);
    ImGui::Separator();
    ImGui::Text("Cube");
    DrawVec3Control("Pos", cubeModTc.Translation);
    if(DrawVec3Control("Scale", cubeModTc.Scale)){
        cubeMod.GetComponent<Engine::RigidbodyComponent>().Shape->Dirty = true;
    }

    if(ImGui::Button("PLAY/STOP", {90, 20})){
        if(m_SceneState == Engine::SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == Engine::SceneState::Play)
            OnSceneStop();
    }
    ImGui::End();
  }

private:
 void OnScenePlay(){
    m_SceneState = Engine::SceneState::Play;
    scene->OnRuntimeStart();
 }
 void OnSceneStop(){
    if(m_SceneState != Engine::SceneState::Play)
        return;
    m_SceneState = Engine::SceneState::Edit;
    scene->OnRuntimeStop();
 }

private:
  Engine::SceneState m_SceneState = Engine::SceneState::Edit;
  Engine::Ref<Engine::Model> model;
  Engine::Ref<Engine::Scene> scene;
  Engine::Entity cam;
  Engine::Entity manEntt;
  Engine::Entity cubeMod;
  glm::vec3 target = glm::vec3(0.0f);
};
