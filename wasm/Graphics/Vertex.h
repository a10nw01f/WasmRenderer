#pragma once

#include <glm/glm.hpp>
#include "Attribute.h"
#include <Meta/MPCore.h>

struct Vertex
{
    glm::vec3 m_Position;
    glm::vec2 m_UV;
    glm::vec3 m_Normal;
};

struct CubeMapVertex
{
    glm::vec3 m_Position;
};

using VertexAttributes = TypeList<VERTEX_ATTRIBUTE(Vertex, m_Position), VERTEX_ATTRIBUTE(Vertex,m_UV), VERTEX_ATTRIBUTE(Vertex, m_Normal)>;
using CubeVertexAttributes = TypeList<VERTEX_ATTRIBUTE(CubeMapVertex, m_Position)>;
