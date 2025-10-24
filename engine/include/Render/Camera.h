#pragma once
#include "Core/Types.h"
#include "Core/Timestep.h"
#include "Events/MouseEvent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine {
    enum class CameraMode { FirstPerson = 0, ThirdPerson = 1, TopDown = 2};

    //Default camera values
const float Default_YAW = -90.0f;
const float Default_PITCH = 0.0f;
const float Default_SPEED = 5.5f;
const float Default_SENSITIVITY = 0.1f;
const float Default_ZOOM = 45.0f;

class Camera {
public:
  enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
 Camera() = default;
 Camera(const glm::mat4& projection) : m_ProjectionMatrix(projection){}
 virtual ~Camera() = default;

  virtual void OnUpdate(Timestep ts){}
  virtual void OnEvent(Event& e){} 
 void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateProjectionMatrix(); }
void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
void SetRotation2(const glm::vec3& rotation) { m_Rotation2 = rotation; RecalculateViewMatrix();}

ProjectionType GetProjectionType() const { return m_ProjectionType; }
void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjectionMatrix(); }

float GetRotation() const { return m_Rotation; }
const glm::vec3& GetRotation2() const { return m_Rotation2;}
const glm::vec3& GetPosition() const { return m_Position; }
const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
const glm::mat4& GetViewProjectionMatrix() const { return /*m_ViewProjectionMatrix*/ m_ProjectionMatrix * m_ViewMatrix; }
glm::vec3 GetUpDirection() const
  {
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::vec3 GetRightDirection() const
  {
    return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
  }

  glm::vec3 GetForwardDirection() const
  {
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
  }

  glm::quat GetOrientation() const
  {
    return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
  }

  float GetPitch() const { return m_Pitch; }
float GetYaw() const { return m_Yaw; }
private:
  virtual void RecalculateViewMatrix(){};
  virtual void RecalculateProjectionMatrix(){}
protected:
glm::vec3 m_Position;
 glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
 glm::mat4 m_ViewMatrix;
 glm::mat4 m_ViewProjectionMatrix;
 glm::vec3 m_Rotation2 = {0, 0, 0};
private:
 float m_Rotation = 0.0f;
 float m_Pitch = 0.0f, m_Yaw = 0.0f;
 ProjectionType m_ProjectionType = ProjectionType::Perspective;
 friend class Camera2D;
 friend class Camera3D;
 friend class SceneCamera;
};

 class  Camera2D : public Camera {
public:        
    Camera2D() = default;
    Camera2D(float left, float right, float bottom, float top);
    void SetProjection(float left, float right, float bottom, float top);		   									
private:				
    void RecalculateViewMatrix();
};

class Camera3D : public Camera
{
public:
    Camera3D();
    ~Camera3D();
    void SetPerspective(float verticalFOV, float nearClip, float farClip);
    void SetOrthographic(float size, float nearClip, float farClip);

    float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
    void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjectionMatrix(); }
    float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
    void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjectionMatrix(); }
    float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
    void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjectionMatrix(); }

    float GetOrthographicSize() const { return m_OrthographicSize; }
    void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjectionMatrix(); }
    float GetOrthographicNearClip() const { return m_OrthographicNear; }
    void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjectionMatrix(); }
    float GetOrthographicFarClip() const { return m_OrthographicFar; }
    void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjectionMatrix(); }

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;

    inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; RecalculateProjectionMatrix(); }

    float GetVerticalFOV() const { return m_PerspectiveFOV; }
    float GetAspectRatio() const { return m_AspectRatio; }
    void SetMode(CameraMode mode) { m_Mode = mode; RecalculateViewMatrix(); }
    void SetTarget(const glm::vec3& target) { m_Target = target; RecalculateViewMatrix(); }
    void SetOffset(const glm::vec3& offset) { m_Offset = offset; RecalculateViewMatrix(); }
    
private:
    void RecalculateProjectionMatrix() override;
    void RecalculateViewMatrix() override;

    bool OnMouseScroll(MouseScrolledEvent& e);

    void MousePan(const glm::vec2& delta);
    void MouseRotate(const glm::vec2& delta);
    void MouseZoom(float delta);

    glm::vec3 CalculatePosition() const;    

    std::pair<float, float> PanSpeed() const;
    float RotationSpeed() const;
    float ZoomSpeed() const;
private:    
    float m_PerspectiveFOV = glm::radians(45.0f);
    float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

    float m_OrthographicSize = 10.0f;
    float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

    float m_AspectRatio = 1.778f;

    CameraMode m_Mode = CameraMode::ThirdPerson;
    glm::vec3 m_Target = glm::vec3(0.0f); // Entity to follow
    glm::vec3 m_Offset = glm::vec3(0.0f, 2.0f, 5.0f); // Third person offset

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
