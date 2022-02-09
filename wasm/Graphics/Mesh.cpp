#include "Mesh.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengles2.h>
#include <Graphics/Vertex.h>

void Mesh::UploadData(const void* data, int size)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    m_Vbo.Reset(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Mesh::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo.Get());
}