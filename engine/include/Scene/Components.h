#pragma once

#include "Core/UUID.h"
#include "Render/Model.h"
#include "Render/Camera.h"
#include "Animation/Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

  struct  CubeComponent{
    glm::vec3 Color;
    CubeComponent() = default;
    CubeComponent(const CubeComponent&) = default;
  };

  struct Camera3DComponent
  {
    Camera3D Camera;
    bool Primary = false; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    Camera3DComponent() = default;
    Camera3DComponent(const Camera3DComponent&) = default;
  };
}
