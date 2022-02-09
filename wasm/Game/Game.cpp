#include "Game.h"
#include "env.h"
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_opengles2.h>
#include <chrono>
#include <Meta/MPCore.h>
#include <Graphics/Vertex.h>
#include <Graphics/Attribute.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Resources/ParseObj.h>
#include <Graphics/Shaders.h>
#include <Math/Math.h>
#include <Graphics/Light.h>

namespace
{
    float NanoSecsToMiliSecs(std::chrono::nanoseconds nano_seconds)
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(nano_seconds).count();
	}

    const static uint8_t kDefaultPixel[] = {255,255,255,255};

    const static int kMaxLights = 32;
}

Game::Game() : 
    m_Width(1600),
    m_Height(900),
    m_Program(kVertexShaderSrc,kFragmentShaderSrc), 
    m_SkyboxProgram(kCubeMapVertexShaderSrc, kSkyboxFragmentShaderSrc),
    m_ShadowMapProgram(kDepthVertexShaderSrc, kEmptyFragmentShaderSrc),
    m_DefaultTexture((void*)&kDefaultPixel, 1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, false)
{
    SetAsRenderTarget();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  	glClearColor(0.0,0.0,0.8,1.0);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CubeMapVertex vertices[] = {
        // positions          
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}}
    };

    m_CubeMesh = std::make_unique<Mesh>(vertices, ArraySize(vertices));

    PointLight point_lights[kMaxLights];

    for(int i = 0; i < kMaxLights; ++i)
    {
        point_lights[i] = PointLight{
            glm::vec3(0.f, 0.f,0.f),
            glm::vec3(100.f, 100.f, 100.f)
        };
    }

    m_PointLights.Allocate(point_lights, kMaxLights);

    DirectionalLight directional_lights[kMaxLights];

    for(int i = 0; i < kMaxLights; ++i)
    {
        directional_lights[i] = DirectionalLight{
            glm::vec3(0.f, 1.f,0.f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        };
    }

    m_DirectionalLights.Allocate(directional_lights, kMaxLights);
}

void Game::OnResize(int width, int height)
{
    m_Camera.SetPerspective(width, height, glm::radians(45.f), 0.1f, 250.f);
    m_Width = width;
    m_Height = height;
}

void Game::HandleInput(float delta_time)
{
    float rotationX = 0.f;
    float rotationY = 0.f;
    auto pos = m_Camera.GetPosition();
    auto dir = m_Camera.GetDirection();
    auto up = m_Camera.GetUp();
    auto right = m_Camera.CalculateRight();

    const float rotation_speed = 1.f / 0.016f;
    const float movement_speed = 1.f / 0.016f;

    auto movement = movement_speed * delta_time;
    auto rotation = rotation_speed * delta_time;

    auto modifiers = SDL_GetModState();
    SDL_Event evnt;
    if(modifiers & KMOD_LSHIFT)
    {
        while (SDL_PollEvent(&evnt))
        {
            if (evnt.type == SDL_KEYDOWN)
            {
                switch (evnt.key.keysym.sym)
                {
                case SDLK_d:
                    rotationY -= rotation;
                    break;
                case SDLK_a:
                    rotationY += rotation;
                    break;
                case SDLK_w:
                    rotationX -= rotation;
                    break;
                case SDLK_s:
                    rotationX += rotation;
                    break;
                case SDLK_SPACE:
                    pos -= up * movement;
                    break;
                }
            }
        }
    }
    else 
    {    
        while (SDL_PollEvent(&evnt))
        {
            if (evnt.type == SDL_KEYDOWN)
            {
                switch (evnt.key.keysym.sym)
                {
                case SDLK_w:
                    pos += dir * movement;
                    break;
                case SDLK_s:
                    pos -= dir * movement;
                    break;
                case SDLK_a:
                    pos -= right * movement;
                    break;
                case SDLK_d:
                    pos += right * movement;
                    break;
                case SDLK_SPACE:
                    pos += up * movement;
                    break;
                }
            }
        }

    }

    auto rotation_matrix = glm::rotate(glm::mat4(), glm::radians(rotationX), right) *   
        glm::rotate(glm::mat4(), glm::radians(rotationY), glm::vec3(0.f, 1.f, 0.f));

    auto new_dir = rotation_matrix * glm::vec4(dir, 0.f);
    
    m_Camera.SetDirection(glm::vec3(new_dir));
    m_Camera.SetPosition(pos);
}

void Game::DrawSkybox()
{
    if(m_SkyBoxTexture)
    {
        auto skybox_matrix = m_Camera.GetUntranslatedCameraMatrix();

        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        m_CubeMesh->Bind();
        m_SkyboxProgram.Use();
        EnableAttributes(CubeVertexAttributes{});
        glUniformMatrix4fv(m_SkyboxProgram.GetUniformLocation("cameraMatrix"), 1, GL_FALSE, &skybox_matrix[0][0]);
        
        m_SkyBoxTexture->Bind();
        glUniform1i(m_SkyboxProgram.GetUniformLocation("skybox"), 0);
        m_SkyboxProgram.SetUniform("exposure", m_Exposure);

        glDrawArrays(GL_TRIANGLES, 0, m_CubeMesh->GetVertexCount());

        DisableAttributes(CubeVertexAttributes{});
        glEnable(GL_DEPTH_TEST);
    }
}

namespace
{
    void BindMeshTexture(TexturedMesh& mesh, std::unique_ptr<Texture2D> TexturedMesh::*texture, Texture2D& defaultTexture)
    {
        if(mesh.*texture)
        {
            (mesh.*texture)->Bind();
        }
        else 
        {
            defaultTexture.Bind();
        }   
    }
}

void Game::DrawShadowMap(const glm::mat4& camera_matrix)
{
    m_ShadowMapProgram.Use();
    for(auto& entity : m_Entities)
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
}

void Game::DrawEntities()
{
    const auto& camera_matrix = m_Camera.GetCameraMatrix();
    auto camera_position = m_Camera.GetPosition();
    m_Program.Use();

    glUniformMatrix4fv(m_Program.GetUniformLocation("viewMatrix"), 1, GL_FALSE, &m_Camera.GetViewMatrix()[0][0]);
    glUniform3f(m_Program.GetUniformLocation("cameraPos"), camera_position.x,camera_position.y,camera_position.z);
    m_Program.SetUniform("exposure", m_Exposure);

    if(m_Prefiltered)
    {
        glActiveTexture(GL_TEXTURE0);
        m_Prefiltered->Bind();
        m_Program.SetUniform("prefilteredMap", 0);
        m_Program.SetUniform("useIBL", (int)true);
    }
    else 
    {
        glActiveTexture(GL_TEXTURE0);
        m_DefaultTexture.Bind();
        m_Program.SetUniform("prefilteredMap", 0);
        m_Program.SetUniform("useIBL", (int)false);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_PointLights.GetBufferID());
    m_Program.BindUniformBlock("PointLights", 0);
    m_Program.SetUniform("pointLightCount", (uint32_t)m_PointLights.GetCount());

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_DirectionalLights.GetBufferID());
    m_Program.BindUniformBlock("DirectionalLights", 1);
    m_Program.SetUniform("directionalLightCount", (uint32_t)m_DirectionalLights.GetCount());

    glActiveTexture(GL_TEXTURE1);
    m_EnvBaker.GetBrdfLut().Bind();
    m_Program.SetUniform("brdfLutMap", 1);

    for(auto& entity : m_Entities)
    {
        if(!entity)
        {
            continue;
        }

        auto& e = *entity;
        
        if(e.m_Mesh)
        {
            e.m_Mesh->m_Mesh->Bind();
            EnableAttributes(VertexAttributes{});
            m_Program.SetUniform("cameraMatrix", camera_matrix * e.m_Transform);
            m_Program.SetUniform("modelMatrix", e.m_Transform);
            m_Program.SetUniform("roughness", e.m_Mesh->m_Roughness);
            m_Program.SetUniform("metallic", e.m_Mesh->m_Metallic);

            std::unique_ptr<Texture2D> TexturedMesh::* mesh_textures[] = {
                &TexturedMesh::m_AlbedoMap,
                &TexturedMesh::m_MetallicMap,
                &TexturedMesh::m_RoughnessMap,
                &TexturedMesh::m_NormalMap
            };

            const char* uniformNames[] = {
                "albedoMap",
                "metallicMap",
                "roughnessMap",
                "normalMap"
            };

            int start_texture = 2;
            
            for(int i = 0; i < ArraySize(mesh_textures); ++i)
            {
                glActiveTexture(GL_TEXTURE0 + start_texture + i);
                BindMeshTexture(*e.m_Mesh, mesh_textures[i], m_DefaultTexture);
                m_Program.SetUniform(uniformNames[i], start_texture + i);
            }
        
            glDrawArrays(GL_TRIANGLES, 0, e.m_Mesh->m_Mesh->GetVertexCount());
        }
    }

    DisableAttributes(VertexAttributes{});
}

void Game::Draw()
{
    m_Camera.Update();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);

    DrawSkybox();
    DrawEntities();

  	SDL_GL_SwapBuffers();
}

void Game::Update()
{
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto deltaTime = NanoSecsToMiliSecs(currentTime - lastFrameTime) / 1000.f;
    lastFrameTime = currentTime;

    HandleInput(deltaTime);
    Draw();
}

int Game::AddMesh(const char* obj)
{
    auto parsed_obj = ParseObj(obj);

    std::vector<Vertex> vertices;
    for(auto index : parsed_obj.m_Indices)
    {
        vertices.emplace_back(Vertex{
                parsed_obj.m_Positions[index.x],
                parsed_obj.m_UVs[index.y],
                parsed_obj.m_Normals[index.z]
            });
    }

    auto textured_mesh = std::make_unique<TexturedMesh>();
    textured_mesh->m_Mesh = std::make_unique<Mesh>(vertices.data(), vertices.size());

    m_Meshes.emplace_back(std::move(textured_mesh));

    return m_Meshes.size() - 1;
}

int Game::CreateEntity()
{   
    for(int i = 0; i < m_Entities.size(); ++i)
    {
        if(!m_Entities[i])
            return i;
    }

    m_Entities.emplace_back(std::make_unique<Entity>());
    return m_Entities.size() - 1;
}

void Game::DeleteEntity(int entity_index)
{
    m_Entities[entity_index].reset();
}

void Game::SetEntityMesh(int entity_index, int mesh_index)
{
    m_Entities[entity_index]->m_Mesh = m_Meshes[mesh_index].get();
}

namespace 
{
    glm::vec3 ValToVec3(emscripten::val value)
    {
        return glm::vec3(
            value["x"].as<float>(),
            value["y"].as<float>(),
            value["z"].as<float>());
    }

    const static float kPI = 3.141592653589793;
    const static float kDegToRad = 1.f / 360.f * 2.f * kPI; 
}

int Game::CreatePointLight()
{
    return m_PointLights.Create();
}

void Game::SetPointLight(int index, emscripten::val& light_settings)
{
    PointLight light = {
        ValToVec3(light_settings["position"]),
        ValToVec3(light_settings["color"]) * light_settings["intensity"].as<float>()
    };

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_PointLights.GetBufferID());
    m_PointLights.Set(light, index);
}

void Game::DeletePointLight(int index)
{
    m_PointLights.Remove(index);
}

int Game::CreateDirectionalLight()
{
    return m_DirectionalLights.Create();
}

void Game::SetDirectionalLight(int index, emscripten::val& light_settings)
{
    DirectionalLight directional_light = {
        glm::normalize(ValToVec3(light_settings["direction"])),
        ValToVec3(light_settings["color"])
    };

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_DirectionalLights.GetBufferID());
    m_DirectionalLights.Set(directional_light, index);
}

void Game::DeleteDirectionalLight(int index)
{
    m_DirectionalLights.Remove(index);
}

void Game::SetEntityTransform(int entity_index, emscripten::val& transform)
{
    m_Entities[entity_index]->m_Transform = CreateTransform(
        ValToVec3(transform["position"]),
        ValToVec3(transform["rotation"]) * kDegToRad,
        ValToVec3(transform["scale"]));
}

void Game::SetMeshTexture(int mesh_index, std::unique_ptr<Texture2D> TexturedMesh::* texture, emscripten::val* image)
{
    auto mesh = m_Meshes[mesh_index].get();
    mesh->*texture = std::make_unique<Texture2D>(image);
}

void Game::SetMeshMaterial(int mesh_index, emscripten::val& material)
{
    auto& mesh = *m_Meshes[mesh_index];
    mesh.m_Roughness = material["roughness"].as<float>();
    mesh.m_Metallic = material["metallic"].as<float>();
}

void Game::SetEnvironment(uint8_t* data, uint32_t size)
{
    m_SkyBoxTexture = LoadHDR(data,size);
    m_Prefiltered = std::make_unique<TextureCube>(m_EnvBaker.BakePrefilter(*m_SkyBoxTexture, *m_CubeMesh));
    SetAsRenderTarget();
}

void Game::SetEnvironmentSettings(emscripten::val& settings)
{
    m_Exposure = settings["exposure"].as<float>();
}

void Game::SetAsRenderTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0,0,m_Width,m_Height);
}