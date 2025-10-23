#pragma once

#include "Render/Camera.h"

namespace Engine {

class SceneCamera : public Camera {
public:
  SceneCamera() = default;
    SceneCamera(float fov, float aspectRatio, float nearClip, float farClip);

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;

    inline float GetDistance() const { return m_Distance; }
    inline void SetDistance(float distance) { m_Distance = distance; }

    inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; RecalculateProjectionMatrix(); }

    
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
    float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
    
    glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

    glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

    float m_Distance = 10.0f;    

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};
}
