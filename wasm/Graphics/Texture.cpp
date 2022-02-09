#include "Texture.h"

#include <env.h>

#include <memory>
#include <stdint.h>
#include <iostream>

#include <glm/glm.hpp>

#include <SDL/SDL_opengles2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <Graphics/GLSLProgram.h>
#include <emscripten.h>

namespace
{
    uint32_t ValToHandle(emscripten::val* value)
    {
        return *reinterpret_cast<uint32_t*>(value);
    }
}

GLuint CreateTexture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    return texture;
}

Texture2D::Texture2D(GLuint textureID) : m_TextureID(textureID)
{

}

Texture2D::Texture2D(emscripten::val* image)
{
    m_TextureID.Reset(CreateTexture());
    Bind(); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    TexImage2DHandle(GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, ValToHandle(image));

    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture2D::Texture2D(void* pixels, int width, int height, uint32_t internal_format, uint32_t format, uint32_t type, bool mipmap)
{
    m_TextureID.Reset(CreateTexture());
    Bind(); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, false, format, type, pixels);

    if(mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID.Get());
}

namespace
{
    struct STBDeleter
    {
        void operator()(void* ptr)
        {
            stbi_image_free(ptr);
        }
    };

    struct TextureLoadResult
    {
        int m_Width;
        int m_Height;
        int m_Components;
        std::unique_ptr<void, STBDeleter> m_Pixels;
    };

    TextureLoadResult LoadImage(const uint8_t* data, uint32_t size, uint32_t required_components = 0)
    {
        TextureLoadResult result;
        stbi_set_flip_vertically_on_load(true);
        result.m_Pixels.reset((void*)stbi_loadf_from_memory(data, size, &result.m_Width, &result.m_Height, &result.m_Components,required_components));

        return result;
    }

    void CopySubImage(glm::vec4* dst, glm::vec4* src, int offsetx, int offsety, int width, int height, int src_width)
    {
        src += offsetx + offsety * src_width;
        dst += (height - 1) * width; 
        for(int i = 0; i < height; ++i)
        {
            memcpy(dst,src, width * sizeof(*src));
            dst -= width;
            src += src_width;
        }
    }
}

std::unique_ptr<TextureCube> LoadHDR(uint8_t* data, uint32_t size)
{
    auto loaded_image = LoadImage(data,size,4);
    glm::ivec2 face_size(loaded_image.m_Width/4, loaded_image.m_Height/3);

    glm::ivec2 faces_offsets[] = {
        {2,1},
        {0,1},
        {1,2},
        {1,0},
        {1,1},
        {3,1}
    };

    auto face_buffer = std::make_unique<glm::vec4[]>(face_size.x * face_size.y);

    uint32_t envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    for(int i = 0; i < 6; i++){
        auto offset = faces_offsets[i] * face_size;
        CopySubImage(face_buffer.get(), (glm::vec4*)loaded_image.m_Pixels.get(), 
            offset.x, offset.y, face_size.x, face_size.y,loaded_image.m_Width);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, 
                 face_size.x, face_size.y, 0, GL_RGBA, GL_FLOAT, face_buffer.get());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return std::make_unique<TextureCube>(envCubemap, face_size.x, face_size.y);
}

TextureCube::TextureCube(uint32_t texture_id, int width, int height) : 
    m_TextureID(texture_id),
    m_Width(width),
    m_Height(height)
{
}

void TextureCube::Bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID.Get());
}