#pragma once

#include "Core/UUID.h"
#include "Render/Model.h"
#include "Render/Camera.h"
#include "Animation/Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Auxiliaries/Physics.h"

namespace Engine {

  struct IDComponent
  {
    UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
  };

  struct TagComponent
  {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
      : Tag(tag) {}
  };

  struct TransformComponent
  {
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
      : Translation(translation) {}

    glm::mat4 GetTransform() const
    {
      glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

      return glm::translate(glm::mat4(1.0f), Translation)
	* rotation
	* glm::scale(glm::mat4(1.0f), Scale);
    }

    float GetRadius() const
    {
      // Assuming you're transforming a unit sphere
      return glm::compMax(Scale) * 0.5f; // max(Scale.x, Scale.y, Scale.z)
    }
  };

  struct  ModelComponent{
    Ref<Model> ModelData;
    std::map<std::string, Ref<Animation>> AnimationData;
    ModelComponent() = default;
    ModelComponent(const ModelComponent&) = default;
  };

  struct Camera3DComponent
  {
    Camera3D Camera;
    bool Primary = false; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    Camera3DComponent() = default;
    Camera3DComponent(const Camera3DComponent&) = default;
  };

  struct CubeComponent{
    glm::vec3 Color;
    CubeComponent() = default;
    CubeComponent(const CubeComponent&) = default;
  };

  struct SphereComponent{
    glm::vec3 Color;
    SphereComponent() = default;
    SphereComponent(const SphereComponent&) = default;
  };

  // PHYSICS 3D
  enum class BodyType { Static, Dynamic, Kinematic };
  struct EShape{
    virtual ~EShape() = default;
    JPH::Ref<JPH::ShapeSettings> Settings;
    JPH::RefConst<JPH::Shape> JPHShape;
    virtual glm::vec3& GetHalfExtents() {
        glm::vec3 null = glm::vec3(0);
        return null; }
    float Dirty = true;
  protected:
   bool box = false;
   bool sphere = false;
   friend class Scene;

  public:
    bool operator!() const {
        return !box && !sphere; // returns true if Shape is null
    }
  };

  struct BoxShape : public EShape {
    glm::vec3 HalfExtents = glm::vec3(0.5f);
    BoxShape(const glm::vec3& halfExtents = glm::vec3(0.5f)){
      box = true;
      HalfExtents = halfExtents;
    }

    glm::vec3& GetHalfExtents() override { return HalfExtents; }
  };

  struct SphereShape : public EShape {
    float Radius = 0.5f;
    SphereShape(float radius = 0.5f){
      sphere = true;
      Radius = radius;
    }
  };
  struct RigidbodyComponent{
    BodyType Type = BodyType::Static;
    EShape* Shape;
    float Mass = 1.0f;
    float LinearDamp = 0.0f;
    float AngularDamp = 0.0f;
    bool Continuous = false; // CCD for fast movers
    float GravityFactor = 1.0f;
    JPH::BodyID ID{};

    RigidbodyComponent() = default;
    RigidbodyComponent(const RigidbodyComponent&) = default;
  };
}
