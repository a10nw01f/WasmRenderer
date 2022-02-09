#include "Camera3D.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera3D::SetPerspective(float width, float height, float rad_fov, float near_z, float far_z)
{
    m_PerspectiveMatrix = glm::perspective(rad_fov, width / height, near_z, far_z);
}

void Camera3D::Update()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
    m_CameraMatrix = m_PerspectiveMatrix * m_ViewMatrix;
}

 glm::mat4 Camera3D::GetUntranslatedCameraMatrix() const
 {
    return m_PerspectiveMatrix * glm::lookAt(glm::vec3(0.f), m_Direction, m_Up);
 }

