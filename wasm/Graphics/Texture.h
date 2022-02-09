#pragma once

#include <emscripten/val.h>
#include <stdint.h>
#include <functional>
#include <GLES3/gl3.h>
#include <Core/Unique.h>
#include <iostream>
#include "GLUnique.h"

GLuint CreateTexture();

class Texture2D
{
public:
    explicit Texture2D(GLuint textureID);
    explicit Texture2D(emscripten::val* image);
    explicit Texture2D(void* pixels, int width, int height, uint32_t internal_format, uint32_t format, uint32_t type, bool mipmap);

    void Bind() const;
    GLuint Get() const { return m_TextureID.Get(); }
private:

    GLUniqueTexture m_TextureID;
};

class TextureCube
{
public:
    explicit TextureCube(uint32_t texture_id, int width, int height);

    void Bind() const;
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
private:
    GLUniqueTexture m_TextureID;
    int m_Width;
    int m_Height;
};

std::unique_ptr<TextureCube> LoadHDR(uint8_t* data, uint32_t size);