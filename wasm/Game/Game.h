#pragma once

#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <Graphics/GLSLProgram.h>
#include <Graphics/Camera3D.h>
#include <Graphics/TexturedMesh.h>
#include <Graphics/EnvironmentBaker.h>
#include <Game/Entity.h>
#include <memory>
#include <vector>
#include <Graphics/Light.h>
#include <Graphics/UniformArray.h>

class Game
{
public:
    Game();

    void Update();

    void OnResize(int width, int height);

    int AddMesh(const char* obj);

    int CreateEntity();

    void DeleteEntity(int entity_index);

    void SetEntityMesh(int entity_index, int mesh_index);

    void SetEntityTransform(int entity_index, emscripten::val& transform);

    void SetMeshTexture(int mesh_index, std::unique_ptr<Texture2D> TexturedMesh::* texture, emscripten::val* image);

    void SetMeshMaterial(int mesh_index, emscripten::val& material);

    void SetEnvironment(uint8_t* data, uint32_t size);

    void SetEnvironmentSettings(emscripten::val& settings);

    int CreatePointLight();

    void SetPointLight(int index, emscripten::val& light_settings);

    void DeletePointLight(int index);

    int CreateDirectionalLight();

    void SetDirectionalLight(int index, emscripten::val& light_settings);

    void DeleteDirectionalLight(int index);
private:
    void HandleInput(float delta_time);

    void SetAsRenderTarget();

    void Draw();
    void DrawSkybox();
    void DrawEntities();
    void DrawShadowMap(const glm::mat4& camera_matix);

    int m_Width = 1600;
    int m_Height = 900;
    float m_Exposure = 1.f;
    Camera3D m_Camera;
    GLSLProgram m_Program;
    GLSLProgram m_SkyboxProgram;
    GLSLProgram m_ShadowMapProgram;
    std::vector<std::unique_ptr<TexturedMesh>> m_Meshes;
    std::vector<std::unique_ptr<Entity>> m_Entities;
    std::unique_ptr<TextureCube> m_SkyBoxTexture;
    std::unique_ptr<TextureCube> m_Prefiltered;
    std::unique_ptr<Mesh> m_CubeMesh;
    EnvironmentBaker m_EnvBaker;
    Texture2D m_DefaultTexture;
    UniformArray<PointLight> m_PointLights;
    UniformArray<DirectionalLight> m_DirectionalLights;
};