#include <emscripten.h>
#include <emscripten/val.h>
#include <memory.h>
#include <iostream>
#include <stdio.h>
#include <Game/Game.h>
#include <Graphics/TexturedMesh.h>
#include "env.h"

Game* g_Game;

namespace
{
    emscripten::val& HandleToValue(uint32_t& handle)
    {
        return *reinterpret_cast<emscripten::val*>(&handle);
    }
}

#define SET_MESH_TEXTURE(name) \
    void EMSCRIPTEN_KEEPALIVE SetMesh##name (int mesh_index, uint32_t image)\
    {\
        g_Game->SetMeshTexture(mesh_index, &TexturedMesh::m_##name, &HandleToValue(image));\
    }

extern "C"
{
    void EMSCRIPTEN_KEEPALIVE Init()
    {
        g_Game = new Game();
    }

    void EMSCRIPTEN_KEEPALIVE OnResize(int width, int height)
    {
        g_Game->OnResize(width, height);
    }

    void EMSCRIPTEN_KEEPALIVE Update()
    {
        g_Game->Update();
    }

    void EMSCRIPTEN_KEEPALIVE Destroy()
    {
        delete g_Game;
    }

    int EMSCRIPTEN_KEEPALIVE AddMesh(const char * value)
    {
        return g_Game->AddMesh(value);
    }

    int EMSCRIPTEN_KEEPALIVE CreateEntity()
    {
        return g_Game->CreateEntity();
    }

    void EMSCRIPTEN_KEEPALIVE DeleteEntity(int entity_index)
    {
        g_Game->DeleteEntity(entity_index);
    }

    void EMSCRIPTEN_KEEPALIVE SetEntityMesh(int entity_index, int mesh_index)
    {
        g_Game->SetEntityMesh(entity_index, mesh_index);
    }

    void EMSCRIPTEN_KEEPALIVE SetEntityTransform(int entity_index, uint32_t transform_handle)
    {
        g_Game->SetEntityTransform(entity_index, HandleToValue(transform_handle));
    }

    SET_MESH_TEXTURE(AlbedoMap)
    SET_MESH_TEXTURE(RoughnessMap)
    SET_MESH_TEXTURE(MetallicMap)
    SET_MESH_TEXTURE(NormalMap)

    void EMSCRIPTEN_KEEPALIVE SetMeshMaterial(int mesh_index, uint32_t handle)
    {
        g_Game->SetMeshMaterial(mesh_index, HandleToValue(handle));
    }

    void EMSCRIPTEN_KEEPALIVE SetEnvironment(uint8_t* data, uint32_t size)
    {
        g_Game->SetEnvironment(data,size);
    }

    void EMSCRIPTEN_KEEPALIVE SetEnvironmentSettings(uint32_t handle)
    {
        g_Game->SetEnvironmentSettings(HandleToValue(handle));
    }

    int EMSCRIPTEN_KEEPALIVE CreatePointLight()
    {
        return g_Game->CreatePointLight();
    }

    void EMSCRIPTEN_KEEPALIVE SetPointLight(int index, uint32_t handle)
    {
        g_Game->SetPointLight(index, HandleToValue(handle));
    }

    void EMSCRIPTEN_KEEPALIVE DeletePointLight(int entity_index)
    {
        g_Game->DeletePointLight(entity_index);
    }

    int EMSCRIPTEN_KEEPALIVE CreateDirectionalLight()
    {
        return g_Game->CreateDirectionalLight();
    }

    void EMSCRIPTEN_KEEPALIVE SetDirectionalLight(int index, uint32_t handle)
    {
        g_Game->SetDirectionalLight(index, HandleToValue(handle));
    }

    void EMSCRIPTEN_KEEPALIVE DeleteDirectionalLight(int entity_index)
    {
        g_Game->DeleteDirectionalLight(entity_index);
    }
}
