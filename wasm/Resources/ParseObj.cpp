#include "ParseObj.h"
#include <stdlib.h>

namespace 
{
    template<class T>
    const T* FindNext(const T* ptr, T value, T terminator = 0)
    {
        while(*ptr != terminator)
        {
            if(*ptr == value)
            {
                return ptr;
            }
            ptr++;
        }

        return nullptr;
    }

    template<class T>
    T ParseVec(const char** str)
    {
        T result;
        char* end;
        result.x = strtof(*str, &end);

        for(int i = 1; i < result.length(); ++i)
        {
            result[i] = strtof(end + 1, &end);
        }

        *str = end;
        return result;
    }

    glm::ivec3 ParseFaceVertex(const char** str)
    {
        glm::ivec3 result(-1,-1,-1);
        char* end;
        result.x = strtol(*str, &end, 10);

        for(int i = 1; i < result.length(); ++i)
        {
            if(*end != '/')
            {
                break;
            }

            result[i] = strtol(end + 1, &end, 10);
        }
        
        *str = end;
        return result - glm::ivec3(1,1,1);
    }
}

ObjResult ParseObj(const char* obj)
{
    ObjResult result;

    auto read_position = obj;
    while(true)
    {
        if(*read_position == 'v')
        {
            if(read_position[1] == 'n')
            {
                read_position += 3;
                result.m_Normals.emplace_back(ParseVec<glm::vec3>(&read_position));
            }
            else if(read_position[1] == 't')
            {
                read_position += 3;
                auto uv = ParseVec<glm::vec2>(&read_position);
                uv.y = 1.f - uv.y;
                result.m_UVs.emplace_back(uv);
            }
            else 
            {
                read_position += 2;
                result.m_Positions.emplace_back(ParseVec<glm::vec3>(&read_position));
            }
        }
        else if(*read_position == 'f')
        {
            read_position += 3;
            result.m_Indices.emplace_back(ParseFaceVertex(&read_position));
            read_position++;
            result.m_Indices.emplace_back(ParseFaceVertex(&read_position));
            read_position++;
            result.m_Indices.emplace_back(ParseFaceVertex(&read_position));
        }

        read_position = FindNext(read_position, '\n', '\0');
        if(!read_position){
            return result;
        }

        read_position++;
    }
    return result;
}