#pragma once
#include "Core/Types.h"
#include "Core/Timestep.h"
#include "Events/MouseEvent.h"

namespace Engine {

    //Default camera values
const float Default_YAW = -90.0f;
const float Default_PITCH = 0.0f;
const float Default_SPEED = 5.5f;
const float Default_SENSITIVITY = 0.1f;
const float Default_ZOOM = 45.0f;

class Camera{
public:
 Camera() = default;
 Camera(const glm::mat4& projection) : m_ProjectionMatrix(projection){}
 virtual ~Camera() = default;
 void SetPosition(const glm::vec3& position) { *m_Position = position; RecalculateViewMatrix(); }
void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
void SetRotation2(const glm::vec3& rotation) { m_Rotation2 = rotation; RecalculateViewMatrix();}

float GetRotation() const { return m_Rotation; }
const glm::vec3& GetRotation2() const { return m_Rotation2;}
const glm::vec3& GetPosition() const { return *m_Position; }
const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
glm::vec3* m_Position = nullptr;
private:
 virtual void RecalculateViewMatrix() {};
protected:
 glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
 glm::mat4 m_ViewMatrix;
 glm::mat4 m_ViewProjectionMatrix;
 glm::vec3 m_Rotation2 = {0, 0, 0};
private:
 float m_Rotation = 0.0f;
 friend class Camera2D;
 friend class Camera3D;
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
    Camera3D() = default;
    Camera3D(float fov, float aspectRatio, float nearClip, float farClip);

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

    inline float GetDistance() const { return m_Distance; }
    inline void SetDistance(float distance) { m_Distance = distance; }

    inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

    glm::vec3 GetUpDirection() const;
    glm::vec3 GetRightDirection() const;
    glm::vec3 GetForwardDirection() const;
    const glm::vec3& GetPosition() const { return m_Position; }
    glm::quat GetOrientation() const;

    float GetPitch() const { return m_Pitch; }
    float GetYaw() const { return m_Yaw; }
private:
    void UpdateProjection();
    void UpdateView();

    bool OnMouseScroll(MouseScrolledEvent& e);

    void MousePan(const glm::vec2& delta);
    void MouseRotate(const glm::vec2& delta);
    void MouseZoom(float delta);

    glm::vec3 CalculatePosition() const;

    std::pair<float, float> PanSpeed() const;
    float RotationSpeed() const;
    float ZoomSpeed() const;
private:
    float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

    glm::mat4 m_ViewMatrix;
    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

    glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

    float m_Distance = 10.0f;
    float m_Pitch = 0.0f, m_Yaw = 0.0f;

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}