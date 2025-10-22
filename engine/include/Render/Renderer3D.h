#pragma once

#include "Core/Types.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

namespace Engine {

    class Renderer3D{
    public:
     static void Init();
     static void Shutdown();

     static void BeginCamera(const Camera3D& camera);
     static void EndCamera();

     static void RenderLight(const glm::vec3& pos);

     static void DrawCube(const glm::mat4& transform, const glm::vec3& color = glm::vec3(1), const float transparancy = 1.0f);
     static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color = glm::vec3(1), const float transparancy = 1.0f);
    };
}