#pragma once

#include <Core/Unique.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include "GLUnique.h"

class GLSLProgram
{
public:
    GLSLProgram(const char* vertex_shader_src, const char* fragment_shader_src);

    void Use();
    int GetUniformLocation(const char* name);
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, unsigned int value);
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, const glm::mat4& matrix);
    void BindUniformBlock(const char* name, int block_index);

private:
    GLUniqueProgram m_Program;
};