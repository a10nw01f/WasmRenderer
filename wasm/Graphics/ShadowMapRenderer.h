#pragma once

#include "GLSLProgram.h"
#include "Texture.h"
#include <vector>
#include "../Game/Entity.h"
#include <memory>

class ShadowMapRenderer
{
public:
    ShadowMapRenderer();

    Texture2D CreateEmptyShadowMap();

    glm::mat4 CalcDirectionalLightMatrix(const glm::vec3& direction) const;

    void DrawShadowMap(Texture2D& shadow_map, const glm::mat4& camera_matrix, const std::vector<std::unique_ptr<Entity>>& entities);

private:
    GLSLProgram m_ShadowMapProgram;
    GLUniqueFrameBuffer m_FrameBuffer;
};