#pragma once

#include "GLSLProgram.h"
#include "Texture.h"
#include "Mesh.h"
#include "GLUnique.h"
#include <Core/Unique.h>
#include <GLES3/gl3.h>

class EnvironmentBaker
{
public:
    EnvironmentBaker();

    TextureCube BakePrefilter(const TextureCube& cubemap, const Mesh& cube_mesh);

    Texture2D& GetBrdfLut() { return m_BrdfLut; }
private:

    Texture2D BakeIntegrateBrdf();

    GLSLProgram m_PrefilterProgram;
    GLSLProgram m_IntegrateBrdfProgram;
    GLUniqueFrameBuffer m_FrameBuffer;
    GLUniqueRenderBuffer m_RenderBuffer;
    Texture2D m_BrdfLut;
};