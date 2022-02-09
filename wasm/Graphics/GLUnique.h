#pragma once

#include <Core/Unique.h>
#include <GLES3/gl3.h>

template<class Deleter>
using GLUniqueHandle = Unique<GLuint, Deleter>;

struct GLProgramDeleter{
    void operator()(GLuint program) { glDeleteProgram(program);}
};

using GLUniqueProgram = GLUniqueHandle<GLProgramDeleter>;

struct BufferDeleter{
    void operator()(GLuint buffer) { glDeleteBuffers(1, &buffer); }
};

using GLUniqueBuffer = GLUniqueHandle<BufferDeleter>; 

struct TextureDeleter{
    void operator()(GLuint texture) { glDeleteTextures(1, &texture); }
};

using GLUniqueTexture = GLUniqueHandle<TextureDeleter>;

struct FrameBufferDeleter{
    void operator()(GLuint fbo) { glDeleteFramebuffers(1, &fbo); }
}; 

using GLUniqueFrameBuffer = GLUniqueHandle<FrameBufferDeleter>;

struct RenderBufferDeleter{
    void operator()(GLuint rbo) { glDeleteRenderbuffers(1, &rbo); }
}; 

using GLUniqueRenderBuffer = GLUniqueHandle<RenderBufferDeleter>;

inline GLuint CreateBuffer(){
    GLuint vbo;
    glGenBuffers(1, &vbo);

    return vbo;
}    

inline GLuint CreateFrameBuffer()
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    return fbo;
}

inline GLuint CreateRenderBuffer()
{
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    return rbo;
}
