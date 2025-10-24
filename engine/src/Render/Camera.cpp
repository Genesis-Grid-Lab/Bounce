#include "epch.h"
#include "Render/Camera.h"
#include "Core/Input.h"

namespace Engine {

  // camera 2d
  Camera2D::Camera2D(float left, float right, float bottom, float top)
  {		
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -10.0f, 10.0f);
    m_ViewMatrix = glm::mat4(1.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
  }
  void Camera2D::RecalculateViewMatrix()
  {		
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
		
    m_ViewMatrix = glm::inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
  }
	
  void Camera2D::SetProjection(float left, float right, float bottom, float top)
  {		
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
  }	


  // camera 3d

  Camera3D::Camera3D()
  {
		
    RecalculateViewMatrix();
  }

  Camera3D::~Camera3D(){}

  void Camera3D::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjectionMatrix();
	}

	void Camera3D::SetOrthographic(float size, float nearClip, float farClip)
	{		
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjectionMatrix();
	}

  void Camera3D::RecalculateProjectionMatrix()
  {
    if (m_ProjectionType == ProjectionType::Perspective)
    {
        m_ProjectionMatrix = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
    }
    else
    {
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;

        m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight,
            orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
    }

    RecalculateViewMatrix();
  }

  void Camera3D::RecalculateViewMatrix()
  {

    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    
    switch (m_Mode)
    {
        case CameraMode::ThirdPerson:
        {
            glm::vec3 behind = glm::normalize(m_Target - (m_Target + m_Offset));
            m_Position = m_Target + m_Offset;
            direction = glm::normalize(m_Target - m_Position);
            break;
        }

        case CameraMode::FirstPerson:
        {
            m_Position = m_Target;
            direction = glm::normalize(glm::vec3(
                cos(m_Rotation2.y) * cos(m_Rotation2.x),
                sin(m_Rotation2.x),
                sin(m_Rotation2.y) * cos(m_Rotation2.x)
            ));
            break;
        }

        case CameraMode::TopDown:
        {
            m_Position = m_Target + glm::vec3(0.0f, m_Offset.y, 0.0f);
            direction = glm::vec3(0.0f, -1.0f, 0.0f); // looking straight down
            break;
        }
    }

    // Build an orthonormal basis to get a stable up vector
    // If direction is nearly parallel to worldUp, pick a different up reference.
    glm::vec3 right = glm::cross(direction, worldUp);
    if (glm::length2(right) < 1e-6f) {
        // direction is nearly parallel to worldUp — choose a fallback up (e.g. +Z)
        right = glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    right = glm::normalize(right);
    glm::vec3 cameraUp = glm::normalize(glm::cross(right, direction));

    // Prevent accidental inversion: make sure cameraUp roughly points same way as worldUp
    if (glm::dot(cameraUp, worldUp) < 0.0f) {
        cameraUp = -cameraUp;
        // Recompute right to keep orthonormal basis consistent
        right = glm::normalize(glm::cross(direction, cameraUp));
    }

    glm::vec3 test = { cameraUp.x, -cameraUp.y, cameraUp.z};
    
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + direction, test);    
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
  }

  std::pair<float, float> Camera3D::PanSpeed() const
  {
    float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return { xFactor, yFactor };
  }

  float Camera3D::RotationSpeed() const
  {
    return 0.8f;
  }

  float Camera3D::ZoomSpeed() const
  {
    
    return 1;
  }

  void Camera3D::OnUpdate(Timestep ts)
  {
    // if (Input::IsKeyPressed(Key::LeftAlt))
    //   {
	// const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
	// glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
	// m_InitialMousePosition = mouse;

	// if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
	//   MousePan(delta);
	// else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
	//   MouseRotate(delta);
	// else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
	//   MouseZoom(delta.y);
    //   }

    if(Input::IsKeyPressed(Key::W)){

    }

    RecalculateProjectionMatrix();
  }

  void Camera3D::OnEvent(Event& e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Camera3D::OnMouseScroll));
  }

  bool Camera3D::OnMouseScroll(MouseScrolledEvent& e)
  {
    float delta = e.GetYOffset() * 0.1f;
    MouseZoom(delta);
    RecalculateProjectionMatrix();
    return false;
  }

  void Camera3D::MousePan(const glm::vec2& delta)
  {
    // auto [xSpeed, ySpeed] = PanSpeed();
    // m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
    // m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
  }

  void Camera3D::MouseRotate(const glm::vec2& delta)
  {
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_Yaw += yawSign * delta.x * RotationSpeed();
    m_Pitch += delta.y * RotationSpeed();
  }

  void Camera3D::MouseZoom(float delta)
  {
    // m_Distance -= delta * ZoomSpeed();
    // if (m_Distance < 1.0f)
    //   {
	// m_FocalPoint += GetForwardDirection();
	// m_Distance = 1.0f;
    //   }
  }

  glm::vec3 Camera3D::CalculatePosition() const
  {
    // return m_FocalPoint - GetForwardDirection() * m_Distance;
    return glm::vec3(0);
  }
}
