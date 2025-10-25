#pragma once
#include "Core/E_Assert.h"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include "Jolt/Core/Core.h"
#include "Jolt/Core/JobSystem.h"
#include "Jolt/Core/Reference.h"
#include <Jolt/Core/IssueReporting.h>
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/ContactListener.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Math/Real.h>

namespace Engine {
// Local Helper
  static inline JPH::RefConst<JPH::Shape> BuildBox(const glm::vec3& size){
    auto res =
        JPH::BoxShapeSettings(JPH::Vec3(size.x, size.y, size.z)).Create();
    return res.IsValid() ? res.Get() : nullptr;
  }

  static inline JPH::RefConst<JPH::Shape> BuildSphere(float r) {
    auto res = JPH::SphereShapeSettings(r).Create();
    return res.IsValid() ? res.Get() : nullptr;
  }

  static inline JPH::RefConst<JPH::Shape> BuildCapsule(float half_h, float r, float offsety = 0){
    JPH::RefConst<JPH::Shape> res = new JPH::CapsuleShape(half_h, r);
    auto rts = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, offsety, 0),JPH::Quat::sIdentity(), res ).Create();
    return rts.IsValid() ? rts.Get() : nullptr;
  }

  static inline glm::quat ToGlmQuat(const JPH::Quat& q) {
    return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); // glm wants (w,x,y,z)
  }
  static inline JPH::Quat ToJoltQuat(const glm::quat& q) {
    return JPH::Quat(q.x, q.y, q.z, q.w); // Jolt ctor is (x,y,z,w)
  }

  static inline JPH::Vec3 ToJoltVec3(const glm::vec3& v){
    return JPH::Vec3(v.x, v.y, v.z);
  }

  static inline glm::vec3 ToGlm(const JPH::Vec3 &v) {
    return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
  }

  static inline void FromJoltTransform(const JPH::RMat44& xf, glm::vec3& outPos, glm::vec3& outEuler)
  {
    const JPH::RVec3 t = xf.GetTranslation();
    outPos = glm::vec3((float)t.GetX(), (float)t.GetY(), (float)t.GetZ());

    const JPH::Quat rq = xf.GetRotation().GetQuaternion();  // Jolt rotation
    const glm::quat gq((float)rq.GetW(), (float)rq.GetX(), (float)rq.GetY(), (float)rq.GetZ());
    outEuler = glm::eulerAngles(gq); // radians
  }

  // Layer that objects can be in, determines which other objects it can collide
  // with Typically you at least want to have 1 layer for moving bodies and 1
  // layer for static bodies, but you can have more layers if you want. E.g. you
  // could have a layer for high detail collision (which is not used by the
  // physics simulation but only if you do collision testing).
  namespace Layers{
    static constexpr JPH::ObjectLayer NON_MOVING  = 0; // static/world
    static constexpr JPH::ObjectLayer MOVING      = 1; // dynamic/kinematic rigidbodies
    static constexpr JPH::ObjectLayer CHARACTER   = 2; // player/NPC controllers
    static constexpr JPH::ObjectLayer NUM_LAYERS  = 3;
  } // namespace Layers

  // Each broadphase layer results in a separate bounding volume tree in the broad
  // phase. You at least want to have a layer for non-moving and moving objects to
  // avoid having to update a tree full of static objects every frame. You can
  // have a 1-on-1 mapping between object layers and broadphase layers (like in
  // this case) but if you have many object layers you'll be creating many broad
  // phase trees, which is not efficient. If you want to fine tune your broadphase
  // layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on
  // the TTY.
  namespace BroadPhaseLayers
  {
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint NUM_LAYERS(2);
  };
  
  /// Class that determines if two object layers can collide
  class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
  {
  public:
      
    bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override {
      switch (a) {
      case Layers::NON_MOVING: return b == Layers::MOVING || b == Layers::CHARACTER;
      case Layers::MOVING:     return true; // collide with NM, MOVING, CHARACTER
      case Layers::CHARACTER:  return b == Layers::NON_MOVING || b == Layers::MOVING; // (no char-char)
      default: JPH_ASSERT(false); return false;
      }
    }
  };

  // BroadPhaseLayerInterface implementation
  // This defines a mapping between object and broadphase layers.
  class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
  {
  public:
    BPLayerInterfaceImpl()
    {
      // Create a mapping table from object to broad phase layer
      mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
      mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
      mObjectToBroadPhase[Layers::CHARACTER]  = BroadPhaseLayers::MOVING;
    }

    [[nodiscard]] JPH::uint GetNumBroadPhaseLayers() const override
    {
      return BroadPhaseLayers::NUM_LAYERS;
    }

    [[nodiscard]] JPH::BroadPhaseLayer GetBroadPhaseLayer(
							  JPH::ObjectLayer inLayer) const override
    {
      JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
      return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(
					       JPH::BroadPhaseLayer inLayer) const override
    {
      switch ((JPH::BroadPhaseLayer::Type)inLayer)
	{
	case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
	  return "NON_MOVING";
	case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
	  return "MOVING";
	default:
	  JPH_ASSERT(false);
	  return "INVALID";
	}
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

  private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    //std::array<JPH::BroadPhaseLayer, Layers::NUM_LAYERS> mObjectToBroadPhase;
  };

  /// Class that determines if an object layer can collide with a broadphase layer
  class ObjectVsBroadPhaseLayerFilterImpl
    : public JPH::ObjectVsBroadPhaseLayerFilter
  {
  public:
    [[nodiscard]] bool ShouldCollide(
				     JPH::ObjectLayer ol,
				     JPH::BroadPhaseLayer bpl) const override
    {
      switch (ol) {
      case Layers::NON_MOVING: return bpl == BroadPhaseLayers::MOVING;
      case Layers::MOVING:     return true;
      case Layers::CHARACTER:  return bpl == BroadPhaseLayers::NON_MOVING || bpl == BroadPhaseLayers::MOVING;
      default: JPH_ASSERT(false); return false;
	  
      }
    }
  };

  // An example contact listener
  class MyContactListener : public JPH::ContactListener
  {
  public:
    // See: ContactListener
    JPH::ValidateResult OnContactValidate(
					  const JPH::Body & /* inBody1 */,
					  const JPH::Body & /* inBody2 */,
					  JPH::RVec3Arg /* inBaseOffset */,
					  const JPH::CollideShapeResult & /* inCollisionResult */) override
    {
      E_CORE_INFO("Contact validate callback");

      // Allows you to ignore a contact before it is created (using layers to not
      // make objects collide is cheaper!)
      return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    void OnContactAdded(const JPH::Body & /* inBody1 */,
			const JPH::Body & /* inBody2 */,
			const JPH::ContactManifold & /* inManifold */,
			JPH::ContactSettings & /* ioSettings */) override
    {
      E_CORE_INFO("A contact was added");
    }

    void OnContactPersisted(const JPH::Body & /* inBody1 */,
			    const JPH::Body & /* inBody2 */,
			    const JPH::ContactManifold & /* inManifold */,
			    JPH::ContactSettings & /* ioSettings */) override
    {
      E_CORE_INFO("A contact was persisted");
    }

    void OnContactRemoved(
			  const JPH::SubShapeIDPair & /* inSubShapePair */) override
    {
      E_CORE_INFO("A contact was removed");
    }
  };

  // An example activation listener
  class MyBodyActivationListener : public JPH::BodyActivationListener
  {
  public:
    void OnBodyActivated(const JPH::BodyID & /* inBodyID */,
			 JPH::uint64 /* inBodyUserData */) override
    {
      E_CORE_INFO("A body got activated");
    }

    void OnBodyDeactivated(const JPH::BodyID & /* &inBodyID */,
			   JPH::uint64 /* inBodyUserData */) override
    {
      E_CORE_INFO("A body went to sleep");
    }
  };

  class PhysicsEngine {
  public:
    PhysicsEngine();
    ~PhysicsEngine();

    PhysicsEngine(const PhysicsEngine &) = delete;
    PhysicsEngine &operator=(const PhysicsEngine &) = delete;

    void Init();

    void StartSimulation();
    bool Step(float dt);
    void Shutdown();

    JPH::PhysicsSystem &System() { return *m_Physics; }
    const JPH::PhysicsSystem &System() const { return *m_Physics; }
    JPH::BodyInterface &Bodies() { return m_Physics->GetBodyInterface(); }

    JPH::JobSystem &JobSystem() { return *m_Jobs; }
    JPH::TempAllocator &TempAllocator() { return *m_Temp_alloc; }
    float FixedStep() const { return m_FixedStep; }

    const ObjectVsBroadPhaseLayerFilterImpl& GetOVBPFilter() const { return *m_Obj_vs_bp_filter;}
    const ObjectLayerPairFilterImpl& GetPairFilter() const { return *m_Obj_vs_obj_filter;}
  private:
    // global Jolt bootstrap with refcount so we don’t double-register/types
    static void InitJoltOnce();
    static void ShutdownJoltOnce();
    static std::atomic<int> s_joltInits;

    // lifetime: these must outlive PhysicsSystem (Jolt keeps refs to them)
    Scope<BPLayerInterfaceImpl>           m_BP_iface;
    Scope<ObjectVsBroadPhaseLayerFilterImpl> m_Obj_vs_bp_filter;
    Scope<ObjectLayerPairFilterImpl>      m_Obj_vs_obj_filter;
    Scope<MyContactListener>              m_Contact_listener;
    Scope<MyBodyActivationListener>       m_Activation_listener;
    // Scope<UEJoltDebugRenderer>            m_DebugRenderer;

    Scope<JPH::TempAllocatorImpl>         m_Temp_alloc;
    Scope<JPH::JobSystemThreadPool>       m_Jobs;
    Scope<JPH::PhysicsSystem>             m_Physics;
    float m_Accumulator = 0.0f;
    float m_FixedStep   = 1.0f / 60.0f;
  };
}
