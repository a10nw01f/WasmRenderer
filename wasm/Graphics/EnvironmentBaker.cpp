#include "EnvironmentBaker.h"
#include "Shaders.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_opengles2.h>
#include <Meta/MPCore.h>
#include <memory>
#include "Vertex.h"

namespace
{
    const glm::mat4 kCubeViews[] = {
        glm::lookAt(glm::vec3(0.f), glm::vec3(1.f,0.f,0.f),glm::vec3(0.f,-1.f,0.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(-1.f,0.f,0.f),glm::vec3(0.f,-1.f,0.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(0.f,1.f,0.f),glm::vec3(0.f,0.f,1.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(0.f,-1.f,0.f),glm::vec3(0.f,0.f,1.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(0.f,0.f,1.f),glm::vec3(0.f,-1.f,0.f)),
        glm::lookAt(glm::vec3(0.f), glm::vec3(0.f,0.f,-1.f),glm::vec3(0.f,-1.f,0.f))
    };

    const glm::mat4 kCubeProjection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 10.f);
}

EnvironmentBaker::EnvironmentBaker() : 
    m_PrefilterProgram(kCubeMapVertexShaderSrc, kPrefilterFragmentShaderSrc),
    m_IntegrateBrdfProgram(kScreenQuadVertexShaderSrc, kIntegrateBrdfFragmentShaderSrc),
    m_FrameBuffer(CreateFrameBuffer()),
    m_RenderBuffer(CreateRenderBuffer()),
    m_BrdfLut(BakeIntegrateBrdf())
{
}

TextureCube EnvironmentBaker::BakePrefilter(const TextureCube& cubemap, const Mesh& cube_mesh)
{
    const static int kPrefilterSize = 1024;

    auto prefilterTextureID = CreateTexture();
    TextureCube prefilterTexture(prefilterTextureID, kPrefilterSize, kPrefilterSize);
    prefilterTexture.Bind();
    
    for (int i = 0; i < ArraySize(kCubeViews); ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, 
            kPrefilterSize, kPrefilterSize, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    m_PrefilterProgram.Use();
    cube_mesh.Bind();
    glActiveTexture(GL_TEXTURE0);
    cubemap.Bind();
    EnableAttributes(CubeVertexAttributes{});
    glUniform1i(m_PrefilterProgram.GetUniformLocation("envMap"), 0);
    m_PrefilterProgram.SetUniform("resolution", 512.f);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER ,m_FrameBuffer.Get());

    const static int kMipLevels = 8;
    int size = kPrefilterSize;
    for(int i = 0; i < kMipLevels; ++i)
    {
        float roughness = (float)i / (float)(kMipLevels - 1);
        glUniform1f(m_PrefilterProgram.GetUniformLocation("roughness"), roughness);
        for(int j = 0; j < ArraySize(kCubeViews); ++j)
        {
            m_PrefilterProgram.SetUniform("cameraMatrix", kCubeProjection * kCubeViews[j]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, prefilterTextureID, i);

            glViewport(0,0,size,size);

            glClearColor(1.f,0.f,1.f,1.f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0, cube_mesh.GetVertexCount());
        }

        size /= 2;
    }

    DisableAttributes(CubeVertexAttributes{});

    return prefilterTexture;
}

Texture2D EnvironmentBaker::BakeIntegrateBrdf()
{
    const static int kTextureSize = 1024;
    Texture2D texture(nullptr, kTextureSize, kTextureSize, GL_RG16F, GL_RG, GL_FLOAT, false);
    texture.Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer.Get());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.Get(), 0);
    glViewport(0,0,kTextureSize,kTextureSize);

    m_IntegrateBrdfProgram.Use();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_DEPTH_TEST);

    return texture;
}
