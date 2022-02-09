#pragma once

#include <glm/glm.hpp>

struct PointLight
{
    alignas(16) glm::vec3 m_Position;
    alignas(16) glm::vec3 m_Color;
};

struct DirectionalLight
{
    alignas(16) glm::vec3 m_Direction;
    alignas(16) glm::vec3 m_Color;
};