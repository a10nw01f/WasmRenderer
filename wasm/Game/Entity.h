#pragma once

#include <glm/glm.hpp>

struct TexturedMesh;

struct Entity
{
    TexturedMesh* m_Mesh = nullptr;
    glm::mat4 m_Transform = glm::mat4(1.f);
};