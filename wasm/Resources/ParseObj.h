#pragma once

#include <glm/glm.hpp>
#include <vector>

struct ObjResult
{
    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec2> m_UVs;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::ivec3> m_Indices;
};

ObjResult ParseObj(const char* obj);
