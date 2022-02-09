#include "GLSLProgram.h"

#include <env.h>
#include <Core/Errors.h>
#include <iostream>
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengles2.h>
#include <vector>

namespace
{
    GLuint CreateShader(const char* src, GLuint type)
    {
        auto shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            FatalError(errorLog.data());
        }

        return shader;
    }
}

GLSLProgram::GLSLProgram(const char* vertex_shader_src, const char* fragment_shader_src)
{
    auto vertex_shader = CreateShader(vertex_shader_src, GL_VERTEX_SHADER);
    auto fragment_shader = CreateShader(fragment_shader_src, GL_FRAGMENT_SHADER);

    m_Program.Reset(glCreateProgram());
    auto program = m_Program.Get();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program); 

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        
        FatalError(infoLog.data());
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void GLSLProgram::Use()
{
    glUseProgram(m_Program.Get());
}

int GLSLProgram::GetUniformLocation(const char* name)
{
    return glGetUniformLocation(m_Program.Get(), name);
}

void GLSLProgram::SetUniform(const char* name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void GLSLProgram::SetUniform(const char* name, unsigned int value)
{
    glUniform1ui(GetUniformLocation(name), value);
}

void GLSLProgram::SetUniform(const char* name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void GLSLProgram::SetUniform(const char* name, const glm::mat4& matrix)
{
     glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void GLSLProgram::BindUniformBlock(const char* name, int block_binding)
{
    auto block_index = glGetUniformBlockIndex(m_Program.Get(), name);
    glUniformBlockBinding(m_Program.Get(), block_index, block_binding);
}
