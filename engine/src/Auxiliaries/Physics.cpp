#include "epch.h"
#include "Core/E_Assert.h"
#include "Auxiliaries/Physics.h"

namespace Engine {

  // Callback for traces, connect this to your own trace function if you have one
  static void TraceImpl(const char *inFMT, ...)
  {
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1'024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    E_CORE_INFO("[Jolt] {}", buffer);
  }

#ifdef JPH_ENABLE_ASSERTS

  // Callback for asserts, connect this to your own assert handler if you have one
  static bool AssertFailedImpl(const char *inExpression,
			       const char *inMessage,
			       const char *inFile,
			       unsigned int inLine)
  {
    // Print to the TTY
    E_CORE_INFO("{}:{}: ({}) {}", inFile, inLine, inExpression, (inMessage != nullptr ? inMessage : ""));
    // std::cout << inFile << ":" << inLine << ": (" << inExpression << ") "
    // 	      << (inMessage != nullptr ? inMessage : "") << std::endl;        

    // Breakpoint
    return true;
  };

#endif // JPH_ENABLE_ASSERTS

  // ---------- static bootstrap ----------
  std::atomic<int> PhysicsEngine::s_joltInits{0};

  void PhysicsEngine::InitJoltOnce() {
    if (s_joltInits.fetch_add(1, std::memory_order_acq_rel) == 0) {
      JPH::RegisterDefaultAllocator();
      JPH::Trace = TraceImpl;
      JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)
        JPH::Factory::sInstance = new JPH::Factory();
      JPH::RegisterTypes();
    }
  }

  void PhysicsEngine::ShutdownJoltOnce() {
    if (s_joltInits.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      JPH::UnregisterTypes();
      delete JPH::Factory::sInstance;
      JPH::Factory::sInstance = nullptr;
    }
  }

  
  // ---------- engine ----------
  PhysicsEngine::PhysicsEngine() = default;
  PhysicsEngine::~PhysicsEngine() { Shutdown();}

  void PhysicsEngine::Init(){

    if (m_Physics)
      return;

    InitJoltOnce();

    // allocators / jobs
    m_Temp_alloc = CreateScope<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
    const int hw = (int)std::thread::hardware_concurrency();
    const int num_threads   = std::max(1, hw - 1);
    const JPH::uint maxJobs = 1024;
    const JPH::uint maxBars = 8;

    m_Jobs = CreateScope<JPH::JobSystemThreadPool>(maxJobs, maxBars, num_threads);

    // interfaces + filters (must outlive PhysicsSystem)
    m_BP_iface            = CreateScope<BPLayerInterfaceImpl>();
    m_Obj_vs_bp_filter    = CreateScope<ObjectVsBroadPhaseLayerFilterImpl>();
    m_Obj_vs_obj_filter   = CreateScope<ObjectLayerPairFilterImpl>();
    m_Contact_listener    = CreateScope<MyContactListener>();
    m_Activation_listener = CreateScope<MyBodyActivationListener>();

    // create physics system
    m_Physics = CreateScope<JPH::PhysicsSystem>();

    // NOTE: tune these for your game
    constexpr uint cMaxBodies               = 1024;
    constexpr uint cNumBodyMutexes          = 0;
    constexpr uint cMaxBodyPairs            = 1024;
    constexpr uint cMaxContactConstraints   = 1024;

    // _debugRenderer = CreateScope<UEJoltDebugRenderer>();
    // JPH::DebugRenderer::sInstance = _debugRenderer.get();
   
    // gravity etc.
    const JPH::Vec3 gravity(0.0f, -9.81f, 0.0f);

    m_Physics->Init(
		   cMaxBodies,
		   cNumBodyMutexes,
		   cMaxBodyPairs,
		   cMaxContactConstraints,
		   *m_BP_iface,
		   *m_Obj_vs_bp_filter,
		   *m_Obj_vs_obj_filter
		   );
    m_Physics->SetGravity(gravity);
    m_Physics->SetBodyActivationListener(m_Activation_listener.get());
    m_Physics->SetContactListener(m_Contact_listener.get());

    // example: floor (optional)
    // {
    //     BodyInterface& bi = Bodies();
    //     RefConst<Shape> floor_shape = new BoxShape(Vec3(50.0f, 1.0f, 50.0f));
    //     BodyCreationSettings bcs(floor_shape, RVec3(0,-1,0), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
    //     BodyID floor = bi.CreateAndAddBody(bcs, EActivation::DontActivate);
    //     (void)floor;
    // }
  }

  bool PhysicsEngine::Step(float dt){
    if (!m_Physics) return false;
    m_Accumulator += dt;
    bool simulated = false;

    // clamp to avoid spiral of death
    const float maxFrame = 0.25f;
    if (m_Accumulator > maxFrame) m_Accumulator = maxFrame;

    while (m_Accumulator >= m_FixedStep) {
      m_Physics->Update(m_FixedStep, 1, m_Temp_alloc.get(), m_Jobs.get());
      m_Accumulator -= m_FixedStep;
      simulated = true;
    }
    E_CORE_INFO("[PHYSICS ENGINE]: {}", m_Accumulator);    
    return simulated;
  }

  void PhysicsEngine::StartSimulation() {
    if(m_Physics) m_Physics->OptimizeBroadPhase();
  }

  void PhysicsEngine::Shutdown(){

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
  }
}
