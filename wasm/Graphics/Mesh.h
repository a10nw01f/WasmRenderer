#pragma once

#include <Core/Unique.h>
#include <GLES3/gl3.h>
#include "GLUnique.h"

struct Vertex;

class Mesh
{
public:
    template<class T>
    Mesh(const T* vertices, int count) : m_VertexCount(count)
    {
        UploadData(vertices, sizeof(T) * count);
    }


    void Bind() const;

    int GetVertexCount() const { return m_VertexCount; }

private:
    struct BufferDeleter
    {
        void operator()(GLuint buffer) { glDeleteBuffers(1, &buffer); }
    };

    void UploadData(const void* data, int size);

    int m_VertexCount;
    GLUniqueBuffer m_Vbo;
};