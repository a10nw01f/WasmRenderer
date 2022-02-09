#pragma once

#include <glm/glm.hpp>

class Camera3D
{
public:
    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
    void SetUp(const glm::vec3& up) { m_Up = up; }
    void SetPerspective(float width, float height, float rad_fov, float near_z, float far_z);
    void Update();
    const glm::mat4& GetCameraMatrix() const { return m_CameraMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetDirection() const { return m_Direction; }
    const glm::vec3& GetUp() const { return m_Up; }
    glm::vec3 CalculateRight() const { return glm::cross(m_Up, m_Direction); }
    glm::mat4 GetUntranslatedCameraMatrix() const;

private:
    glm::vec3 m_Position = glm::vec3(0,0,0);
    glm::vec3 m_Direction = glm::vec3(0,0,-1);
    glm::vec3 m_Up = glm::vec3(0,1,0);
    glm::mat4 m_PerspectiveMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_CameraMatrix;
};