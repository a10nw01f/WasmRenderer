#pragma once

#include "Mesh.h"
#include <memory>
#include <Graphics/Texture.h>

struct TexturedMesh
{
    float m_Roughness = 0.f;
    float m_Metallic = 0.f;
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Texture2D> m_AlbedoMap;
    std::unique_ptr<Texture2D> m_NormalMap;
    std::unique_ptr<Texture2D> m_RoughnessMap;
    std::unique_ptr<Texture2D> m_MetallicMap;
};
