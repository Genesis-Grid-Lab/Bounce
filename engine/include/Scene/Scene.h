#pragma once

#include "Core/UUID.h"
#include "Core/Timestep.h"
#include "Render/Camera.h"
#include "SceneCamera.h"
#include <entt/entt.hpp>

namespace Engine {

class Entity;

class Scene {
public:
  Scene();
  ~Scene();

  Entity CreateEntity(const std::string &name = std::string());
  Entity CreateEntityWithUUID(UUID uuid,
                              const std::string &name = std::string());
  void DestroyEntity(Entity entity);
  void DestroyEntityNow(Entity entity);
  void FlushEntityDestruction();

  void OnUpdate(Timestep ts);

  template<typename Entt, typename Comp, typename Task>
    void ViewEntity(Task&& task){
      // E_CORE_ASSERT(std::is_base_of<Entity, Entt>::value, "error viewing entt");
      m_Registry.view<Comp>().each([this, &task] 
				   (const auto entity, auto& comp) 
      { 
	// task(std::move(Entt(&m_Registry, entity)), comp);
	task(std::move(Entt(entity, this)), comp);
      });
  }

  void TogglePlaying() { 
    if(m_Playing)
      m_Playing = false;
    else if(m_Playing == false)
      m_Playing = true;
  }

private:
  template <typename T> void OnComponentAdded(Entity entity, T &component);

private:
  entt::registry m_Registry;
  bool m_Playing = false;
  std::vector<entt::entity> m_DestroyQueue;
  SceneCamera m_SceneCam;
  Camera m_MainCam;
  friend class Entity;
  
};
}
