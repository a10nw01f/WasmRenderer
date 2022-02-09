#include "ShadowMapRenderer.h"
#include "Shaders.h"
#include "Vertex.h"
#include "Mesh.h"
#include "TexturedMesh.h"
#include <GLES3/gl3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
    const static int kShadowMapSize = 1024;
}

ShadowMapRenderer::ShadowMapRenderer() :
    m_ShadowMapProgram(kDepthVertexShaderSrc, kEmptyFragmentShaderSrc),
    m_FrameBuffer(CreateFrameBuffer())
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer.Get());
    GLenum buffer = GL_NONE;
    glDrawBuffers(1, &buffer);
    glReadBuffer(GL_NONE);
}

Texture2D ShadowMapRenderer::CreateEmptyShadowMap()
{
    return Texture2D(nullptr, kShadowMapSize, kShadowMapSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, false);
}

void ShadowMapRenderer::DrawShadowMap(Texture2D& shadow_map, const glm::mat4& camera_matrix, const std::vector<std::unique_ptr<Entity>>& entities)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer.Get());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadow_map.Get(), 0);
    glViewport(0,0,kShadowMapSize,kShadowMapSize);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_ShadowMapProgram.Use();
    for(auto& entity : entities)
    {
        if(!entity || !entity->m_Mesh)
            continue;

        auto& e = *entity;

        e.m_Mesh->m_Mesh->Bind();
        EnableAttributes(VertexAttributes{});
        m_ShadowMapProgram.SetUniform("cameraMatrix", camera_matrix * e.m_Transform);
        glDrawArrays(GL_TRIANGLES, 0, e.m_Mesh->m_Mesh->GetVertexCount());
    }
    DisableAttributes(VertexAttributes{});
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 ShadowMapRenderer::CalcDirectionalLightMatrix(const glm::vec3& direction) const
{
    auto pos = direction * -10000.f;
    glm::vec3 up = fabsf(direction.y) > 0.999 ? glm::vec3(0.f,0.f,1.f) : glm::vec3(0.f,1.f,0.f);
    
    return glm::ortho(0.f, (float)kShadowMapSize, 0.f, (float)kShadowMapSize) * glm::lookAt(pos, pos + direction, up);
}
