#pragma once

#include "GLUnique.h"
#include <GLES3/gl3.h>
#include <vector>

template<class T>
struct UniformArray
{
public:
    void Allocate(const T* data, int count)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer.Get());
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * count, data, GL_STATIC_DRAW);
    }

    int Create()
    {
        int slot = m_DenseIDs.size();
        for(int i = 0; i < m_SparseIDs.size(); ++i)
        {
            if(m_SparseIDs[i] == -1)
            {
                m_SparseIDs[i] = slot;
                m_DenseIDs.push_back(i);
                return i;
            }
        }

        m_SparseIDs.push_back(slot);
        m_DenseIDs.push_back(slot);
        return slot;
    }

    void Set(const T& value, int id)
    {
        auto index = m_SparseIDs[id];
        glBindBuffer(m_Buffer.Get(), GL_UNIFORM_BUFFER);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(T) * index, sizeof(T), (void*)&value);
    }

    void Remove(int id)
    {
        auto index = m_SparseIDs[id];
        m_SparseIDs[id] = -1;
        
        auto last_id = m_DenseIDs.back();
        m_DenseIDs.pop_back();
        
        if(m_DenseIDs.size() == index)
        {
            return;
        }

        m_SparseIDs[last_id] = index;

        glBindBuffer(m_Buffer.Get(), GL_UNIFORM_BUFFER);
        glCopyBufferSubData(GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER, sizeof(T) * m_DenseIDs.size(), sizeof(T) * index, sizeof(T));
    }

    GLuint GetBufferID() const { return m_Buffer.Get(); }

    uint32_t GetCount() const { return m_DenseIDs.size(); }

private:
    GLUniqueBuffer m_Buffer = GLUniqueBuffer(CreateBuffer());
    std::vector<int> m_SparseIDs;
    std::vector<int> m_DenseIDs;
};