#include "Core/Timestep.h"
#include "Render/Renderer3D.h"
#include "epch.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Render/RenderCommand.h"
#include "Auxiliaries/Physics.h"
#include "Core/E_Assert.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using uint = unsigned int;

namespace Engine {

Scene::Scene() {
  m_SceneCam = SceneCamera(30.0f, 1.778f, 0.1f, 2000.0f);
  m_Physics3D.Init();
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

  void Scene::BuildRigidBodyForEntity(Entity e){
    auto& rbc = e.GetComponent<RigidbodyComponent>();
    auto& tr = e.GetComponent<TransformComponent>();
    auto& EShape = rbc.Shape;
    if(!EShape){
      E_CORE_ERROR("No SHape in rigid body component: {}", e.GetName());
      return;
    }
    if(EShape->box){
      if(!EShape->JPHShape || EShape->Dirty)
      {
        EShape->JPHShape = BuildBox(dynamic_cast<BoxShape*>(EShape)->HalfExtents);
        EShape->Dirty = false;
        E_CORE_INFO("BOX");
      }
    }
    if(EShape->sphere){
      if(!EShape->JPHShape || EShape->Dirty)
      {
        EShape->JPHShape = BuildSphere(dynamic_cast<SphereShape*>(EShape)->Radius);
        EShape->Dirty = false;
        E_CORE_INFO("SPHERE");
      }
    }

    JPH::EMotionType motion = JPH::EMotionType::Dynamic;
    JPH::ObjectLayer layer  = Layers::MOVING;
    switch (rbc.Type) {
    case BodyType::Static:    motion = JPH::EMotionType::Static;    layer = Layers::NON_MOVING; break;
    case BodyType::Kinematic: motion = JPH::EMotionType::Kinematic; layer = Layers::MOVING;     break;
    case BodyType::Dynamic:
      motion = JPH::EMotionType::Dynamic;
      layer = Layers::MOVING;
      break;
    default:
      E_CORE_WARN("[BuildRigidBodyForEntity]: unknown BodyType");
      break;
    }

    E_CORE_INFO("[BuildRigidBodyForEntity glm pos] {},{},{}", tr.Translation.x, tr.Translation.y, tr.Translation.z);
    const JPH::RVec3 pos(tr.Translation.x, tr.Translation.y, tr.Translation.z);
    E_CORE_INFO("[BuildRigidBodyForEntity JPH pos] {},{},{}", pos.GetX(), pos.GetY(), pos.GetZ());
    const glm::quat gp = glm::quat(tr.Rotation);
    const JPH::Quat  rot = ToJoltQuat(gp);

    JPH::BodyCreationSettings bcs(EShape->JPHShape, pos, rot, motion, layer);
    bcs.mAllowSleeping   = true;
    bcs.mLinearDamping   = rbc.LinearDamp;
    bcs.mAngularDamping  = rbc.AngularDamp;
    bcs.mGravityFactor   = rbc.GravityFactor;
    bcs.mMotionQuality   = rbc.Continuous ? JPH::EMotionQuality::LinearCast
      : JPH::EMotionQuality::Discrete;
    bcs.mUserData = (JPH::uint64)(entt::entity)e; // map contacts -> entity

    auto &bi = m_Physics3D.Bodies();
    auto &sys = m_Physics3D.System();
    rbc.ID = bi.CreateAndAddBody(bcs, JPH::EActivation::Activate);
    if (rbc.ID.IsInvalid()) {
      E_CORE_ERROR("[BuildRigidBodyForEntity]: CreateAndAddBody failed");
      return;
    }

    if (rbc.Type == BodyType::Dynamic && rbc.Mass > 0.0f) {
      // Optional: set mass properties (or leave Jolt to compute from shape density)
      bi.SetLinearVelocity(rbc.ID, JPH::Vec3(0,0,0));
      bi.SetAngularVelocity(rbc.ID, JPH::Vec3(0,0,0));
    }

    // Extra sanity: verify the body truly exists & inspect it.
    {
      const JPH::BodyLockInterfaceNoLock &bli = sys.GetBodyLockInterfaceNoLock();
      JPH::BodyLockRead lock(bli, rbc.ID);
      if (!lock.Succeeded()) {
        E_CORE_ERROR("[BuildRigidBodyForEntity]: CreateAndAddBody - Body lock failed right after creation (not added?)");
        return;
      }
      const JPH::Body &b = lock.GetBody();
      E_CORE_INFO("[BuildRigidBodyForEntity]: Body created - id={} motion={} grav={} added={} active={}",
		   (unsigned)rbc.ID.GetIndexAndSequenceNumber(),
		   (int)b.GetMotionType(),
		   8,
		   bi.IsAdded(rbc.ID) ? 1 : 0,
		   bi.IsActive(rbc.ID) ? 1 : 0);
    }

    if (!bi.IsAdded(rbc.ID)) {
	  E_CORE_ERROR("[BuildRigidBodyForEntity]: {} is not Added with id {}", e.GetName(), (unsigned)rbc.ID.GetIndexAndSequenceNumber());
      return;
    }else {
      E_CORE_INFO("[BuildRigidBodyForEntity] {} Added with id {}", e.GetName(), (unsigned)rbc.ID.GetIndexAndSequenceNumber());
    }
  }

  void Scene::BuildShape(){
      ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto& rbc){
        
        auto& tr = entity.template GetComponent<TransformComponent>();
        auto& EShape = rbc.Shape;
        if(!EShape){
          E_CORE_ERROR("No SHape in rigid body component: {}", entity.GetName());
          return;
        }
        if(EShape->box){
          if(!EShape->JPHShape || EShape->Dirty)
          {
            EShape->JPHShape = BuildBox(dynamic_cast<BoxShape*>(EShape)->HalfExtents);
            EShape->Dirty = false;
            E_CORE_INFO("BOX");
          }
        }
        if(EShape->sphere){
          if(!EShape->JPHShape || EShape->Dirty)
          {
            EShape->JPHShape = BuildSphere(dynamic_cast<SphereShape*>(EShape)->Radius);
            EShape->Dirty = false;
            E_CORE_INFO("SPHERE");
          }
        }
      });
  }

  void Scene::OnRuntimeStart(){
    m_Physics3D.Init();
    m_Physics3D.StartSimulation();

    auto& sys = m_Physics3D.System();
    auto& bi = m_Physics3D.Bodies();

    //Rigid body
    ViewEntity<Entity, RigidbodyComponent>([&](auto e, auto& rbc) {
      auto &tr = e.template GetComponent<TransformComponent>();
      if(rbc.ID.IsInvalid()){
        BuildRigidBodyForEntity(e);
      }

      if (!rbc.ID.IsInvalid()) {
        m_Physics3D.Bodies().SetPositionAndRotation(
						    rbc.ID,
						    JPH::RVec3(tr.Translation.x, tr.Translation.y, tr.Translation.z),
						    ToJoltQuat(glm::quat(tr.Rotation)),
						    JPH::EActivation::Activate
						    );
      }else {
	      E_CORE_WARN("[OnRuntimeStart]: no valid ID in RigidbodyComponent");
      }
    });
  }
  void Scene::OnRuntimeStop(){        
      auto& sys = m_Physics3D.System();
      auto& bi = m_Physics3D.Bodies();

      ViewEntity<Entity, RigidbodyComponent>([&](auto e, auto& rb) {
          // if (e.template HasComponent<CharacterComponent>()) return;
          if (rb.ID.IsInvalid()) 
          {
              E_CORE_ERROR("INVALID");
              return;
          }

          auto &tr = e.template GetComponent<TransformComponent>();
          
          bi.RemoveBody(rb.ID);
          bi.DestroyBody(rb.ID);
          
          if (rb.Type == BodyType::Dynamic) {
        // physics -> ECS
        // const JPH::RMat44 xf = body.GetWorldTransform();
        const JPH::RMat44 xf = bi.GetWorldTransform(rb.ID);
        const JPH::RVec3 p = xf.GetTranslation();        
        // const JPH::RVec3 p = bi.GetCenterOfMassPosition(rb.ID);
              const JPH::Quat q = bi.GetRotation(rb.ID);
            E_CORE_INFO("[PhysicsUpdate]: RigidbodyComponent JPH {} {} {}", p.GetX(), p.GetY(), p.GetZ());
              tr.Translation = {(float)p.GetX(), (float)p.GetY(), (float)p.GetZ()};
            E_CORE_INFO("[PhysicsUpdate]: RigidbodyComponent TRANS {} {} {}", tr.Translation.x, tr.Translation.y, tr.Translation.z);
        tr.Rotation    = glm::eulerAngles(ToGlmQuat(q)); // if you store Euler
            } else if (rb.Type == BodyType::Kinematic) {
        // ECS -> physics
        bi.SetPositionAndRotation(
                rb.ID,
                JPH::RVec3(tr.Translation.x, tr.Translation.y, tr.Translation.z),
                ToJoltQuat(glm::quat(tr.Rotation)),
                JPH::EActivation::Activate
                );
            }
          rb.ID = {};
      });
      
  }
  void Scene::PhysicsUpdate(float dt) {
    
    auto& sys = m_Physics3D.System();
    auto& bi  = m_Physics3D.Bodies();

    m_Physics3D.Step(dt);

    auto &bli = sys.GetBodyLockInterface(); 

     ViewEntity<Entity, RigidbodyComponent>([&](auto e, auto &rb) {
      if (rb.ID.IsInvalid()){
	      E_CORE_ERROR("[PhysicsUpdate]: RigidbodyComponent ID for {} with id={} is invalid", e.GetName(), (uint)rb.ID.GetIndexAndSequenceNumber());
        return;
      }

      // const JPH::Body &body = lock.GetBody();
      auto &tr = e.template GetComponent<TransformComponent>();

      if (rb.Type == BodyType::Dynamic) {
        // physics -> ECS
        // const JPH::RMat44 xf = body.GetWorldTransform();
        const JPH::RMat44 xf = bi.GetWorldTransform(rb.ID);
        const JPH::RVec3 p = xf.GetTranslation();        
        // const JPH::RVec3 p = bi.GetCenterOfMassPosition(rb.ID);
              const JPH::Quat q = bi.GetRotation(rb.ID);
            E_CORE_INFO("[PhysicsUpdate]: RigidbodyComponent JPH {} {} {}", p.GetX(), p.GetY(), p.GetZ());
              tr.Translation = {(float)p.GetX(), (float)p.GetY(), (float)p.GetZ()};
            E_CORE_INFO("[PhysicsUpdate]: RigidbodyComponent TRANS {} {} {}", tr.Translation.x, tr.Translation.y, tr.Translation.z);
        tr.Rotation    = glm::eulerAngles(ToGlmQuat(q)); // if you store Euler
            } else if (rb.Type == BodyType::Kinematic) {
        // ECS -> physics
        bi.SetPositionAndRotation(
                rb.ID,
                JPH::RVec3(tr.Translation.x, tr.Translation.y, tr.Translation.z),
                ToJoltQuat(glm::quat(tr.Rotation)),
                JPH::EActivation::Activate
                );
            }
      // Static: no per-frame push
    });    
    
  }
  
  void Scene::OnUpdateRuntime(Timestep ts) {
    glm::vec4 clearColor = {0.5f, 0.3f, 0.1f, 1.0f};
    RenderCommand::SetClearColor(clearColor);
    RenderCommand::Clear();

    PhysicsUpdate(ts);

    ViewEntity<Entity, Camera3DComponent>([this](auto entity, auto& comp) {
      auto& transform = entity.template GetComponent<TransformComponent>();
      comp.Camera.SetPerspective(30.0f, 0.1f, 2000.0f);
      comp.Camera.SetMode(CameraMode::ThirdPerson);
      comp.Camera.SetPosition(transform.Translation);
      if (comp.Primary) {
	m_MainCam = comp.Camera;
      }
    });
        

    Renderer3D::BeginCamera(m_MainCam);

    Engine::Renderer3D::RenderLight({1, 0, 0});


    ViewEntity<Entity, ModelComponent>([this, &ts](auto entity, auto &comp) {
      auto &transform = entity.template GetComponent<TransformComponent>();

      Renderer3D::DrawModel(comp.ModelData, transform.GetTransform());
      Renderer3D::RunAnimation(comp.AnimationData["idle"], ts);
    });

    ViewEntity<Entity, CubeComponent>([this](auto entity, auto& comp){
      auto &transform = entity.template GetComponent<TransformComponent>();
      Renderer3D::DrawCube(transform.GetTransform(), comp.Color);
    });

    ViewEntity<Entity, SphereComponent>([this](auto entity, auto& comp){
      auto &transform = entity.template GetComponent<TransformComponent>();
      Renderer3D::DrawSphere(transform.GetTransform(), comp.Color);
    });

    m_MainCam.OnUpdate(ts);
    ViewEntity<Entity, Camera3DComponent>([this, &ts](auto entity, auto& comp) {
      auto& transform = entity.template GetComponent<TransformComponent>();
      if (comp.Primary) {
	comp.Camera.OnUpdate(ts);
      }
    });    

    Renderer3D::EndCamera();

    FlushEntityDestruction();
  }

  void Scene::OnUpdate(Timestep ts){
    glm::vec4 clearColor = {0.5f, 0.3f, 0.1f, 1.0f};
    RenderCommand::SetClearColor(clearColor);
    RenderCommand::Clear();

    m_MainCam = m_SceneCam;    

    Renderer3D::BeginCamera(m_MainCam);

    Engine::Renderer3D::RenderLight({1, 0, 0});


    ViewEntity<Entity, Camera3DComponent>([this](auto entity, auto& comp) {
      auto& transform = entity.template GetComponent<TransformComponent>();
      // Renderer3D::DrawCameraFrustum(comp.Camera);
    });    

    ViewEntity<Entity, ModelComponent>([this, &ts](auto entity, auto &comp) {
      auto &transform = entity.template GetComponent<TransformComponent>();

      Renderer3D::DrawModel(comp.ModelData, transform.GetTransform());
      Renderer3D::RunAnimation(comp.AnimationData["idle"], ts);
    });

    
    ViewEntity<Entity, CubeComponent>([this](auto entity, auto& comp){
      auto &transform = entity.template GetComponent<TransformComponent>();
      Renderer3D::DrawCube(transform.GetTransform(), comp.Color);
    });

    ViewEntity<Entity, SphereComponent>([this](auto entity, auto& comp){
      auto &transform = entity.template GetComponent<TransformComponent>();
      Renderer3D::DrawSphere(transform.GetTransform(), comp.Color);
    });

    // BuildShape();

    ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto& comp){
      auto &transform = entity.template GetComponent<TransformComponent>();
      if(comp.Shape){
        if(comp.Shape->box){

          if(!comp.Shape->JPHShape || comp.Shape->Dirty)
          {
            comp.Shape->JPHShape = BuildBox({0.5f, 0.5f, 0.5f});
            comp.Shape->Dirty = false;            
          }
          if(comp.Shape->JPHShape){
            const auto& box = comp.Shape->JPHShape->GetLocalBounds();
            Renderer3D::DrawWireCube({transform.Translation.x, transform.Translation.y, transform.Translation.z}, 
          {box.GetSize().GetX(), box.GetSize().GetY(), box.GetSize().GetZ()}, 
          {0,1,0}, 0.5f);
          }
        }

        if(comp.Shape->sphere){

        }
      }
    });

    m_SceneCam.OnUpdate(ts);

    Renderer3D::EndCamera();

    FlushEntityDestruction();
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

  template <>
  void Scene::OnComponentAdded<Camera3DComponent>(Entity entity, Camera3DComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<RigidbodyComponent>(Entity entity, RigidbodyComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<CubeComponent>(Entity entity, CubeComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<SphereComponent>(Entity entity, SphereComponent& component)
  {}
}
