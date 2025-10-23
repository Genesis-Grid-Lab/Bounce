#include "epch.h"
#include "Scene/SceneCamera.h"
#include "Core/Input.h"

namespace Engine {

    SceneCamera::SceneCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
  {
		
    RecalculateViewMatrix();
  }

  void SceneCamera::RecalculateProjectionMatrix()
  {
    m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio,
                                          m_NearClip, m_FarClip);
    RecalculateViewMatrix();
  }

  void SceneCamera::RecalculateViewMatrix()
  {
    // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
    m_Position = CalculatePosition();

    glm::quat orientation = GetOrientation();
    m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
    m_ViewMatrix = glm::inverse(m_ViewMatrix);
  }

  std::pair<float, float> SceneCamera::PanSpeed() const
  {
    float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return { xFactor, yFactor };
  }

  float SceneCamera::RotationSpeed() const
  {
    return 0.8f;
  }

  float SceneCamera::ZoomSpeed() const
  {
    float distance = m_Distance * 0.2f;
    distance = std::max(distance, 0.0f);
    float speed = distance * distance;
    speed = std::min(speed, 100.0f); // max speed = 100
    return speed;
  }

  void SceneCamera::OnUpdate(Timestep ts)
  {
    if (Input::IsKeyPressed(Key::LeftAlt))
      {
	const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
	glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
	m_InitialMousePosition = mouse;

	if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
	  MousePan(delta);
	else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
	  MouseRotate(delta);
	else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
	  MouseZoom(delta.y);
      }

    RecalculateProjectionMatrix();
  }

  void SceneCamera::OnEvent(Event& e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(SceneCamera::OnMouseScroll));
  }

  bool SceneCamera::OnMouseScroll(MouseScrolledEvent& e)
  {
    float delta = e.GetYOffset() * 0.1f;
    MouseZoom(delta);
    RecalculateProjectionMatrix();
    return false;
  }

  void SceneCamera::MousePan(const glm::vec2& delta)
  {
    auto [xSpeed, ySpeed] = PanSpeed();
    m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
    m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
  }

  void SceneCamera::MouseRotate(const glm::vec2& delta)
  {
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_Yaw += yawSign * delta.x * RotationSpeed();
    m_Pitch += delta.y * RotationSpeed();
  }

  void SceneCamera::MouseZoom(float delta)
  {
    m_Distance -= delta * ZoomSpeed();
    if (m_Distance < 1.0f)
      {
	m_FocalPoint += GetForwardDirection();
	m_Distance = 1.0f;
      }
  }

  glm::vec3 SceneCamera::CalculatePosition() const
  {
    return m_FocalPoint - GetForwardDirection() * m_Distance;
  }
}