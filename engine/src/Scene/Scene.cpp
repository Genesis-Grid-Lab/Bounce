#include "Core/Timestep.h"
#include "Render/Renderer3D.h"
#include "epch.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Render/RenderCommand.h"
#include "Core/E_Assert.h"

namespace Engine {

Scene::Scene() {
  m_SceneCam = Camera3D(30.0f, 1.778f, 0.1f, 2000.0f);
}
Scene::~Scene() {}

  template<typename Component>
  static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
  {
    auto view = src.view<Component>();
    for (auto e : view)
      {
	UUID uuid = src.get<IDComponent>(e).ID;
	E_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
	entt::entity dstEnttID = enttMap.at(uuid);

	auto& component = src.get<Component>(e);
	dst.emplace_or_replace<Component>(dstEnttID, component);
      }
  }

  template<typename Component>
  static void CopyComponentIfExists(Entity dst, Entity src)
  {
    if (src.HasComponent<Component>())
      dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
  }

  Entity Scene::CreateEntity(const std::string& name)
  {
    return CreateEntityWithUUID(UUID(), name);
  }

  Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
  {
    Entity entity = { m_Registry.create(), this };
    // entity.AddComponent<IDComponent>(uuid);
    auto &id = entity.AddComponent<IDComponent>();
    id.ID = uuid;
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;
    return entity;
  }

   void Scene::DestroyEntityNow(Entity entity) { m_Registry.destroy(entity); }

  void Scene::DestroyEntity(Entity entity){
    // Queue for destruction; actual registry destroy happens in
    // FlushEntityDestruction()
    m_DestroyQueue.push_back((entt::entity)entity);
  }

  void Scene::FlushEntityDestruction()
  {
    if (m_DestroyQueue.empty()) return;    
    for (auto e : m_DestroyQueue)
      if (m_Registry.valid(e))
	m_Registry.destroy(e);

    m_DestroyQueue.clear();
  }

  void Scene::OnUpdate(Timestep ts){
    glm::vec4 clearColor = {0.5f, 0.3f, 0.1f, 1.0f};
    RenderCommand::SetClearColor(clearColor);
    RenderCommand::Clear();

    if(!m_Playing)Renderer3D::BeginCamera(m_SceneCam);

    Engine::Renderer3D::RenderLight({0, 0, 0});

    ViewEntity<Entity, ModelComponent>([this](auto entity, auto &comp) {
      auto &transform = entity.template GetComponent<TransformComponent>();
      
      Renderer3D::DrawModel(comp.ModelData, transform.GetTransform());
    });

    m_SceneCam.OnUpdate(ts);
    Renderer3D::EndCamera();
  }

  template<typename T>
    void  Scene::OnComponentAdded(Entity entity, T& component)
  {
    // static_assert(false);
  }

  template<>
    void  Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
  {
  }

  template<>
    void  Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
  {
  }

  template <>
  void Scene::OnComponentAdded<ModelComponent>(Entity entity,
                                               ModelComponent &component)
  {}     
}
