#include "Shaders.h"

#define STRINGIFY(x) #x

const char* kVertexShaderSrc = 
    #include "Shaders/default.vert"
;

const char* kFragmentShaderSrc = 
    #include "Shaders/default.frag"
;


const char* kCubeMapVertexShaderSrc = 
    #include "Shaders/cubemap.vert"
;

const char* kSkyboxFragmentShaderSrc = 
    #include "Shaders/skybox.frag"
;

const char* kPrefilterFragmentShaderSrc = 
    #include "Shaders/prefilter.frag"
;

const char* kIntegrateBrdfFragmentShaderSrc = 
    #include "Shaders/integrateBrdf.frag"
;

const char* kScreenQuadVertexShaderSrc = 
    #include "Shaders/screenQuad.vert"
;

const char* kEmptyFragmentShaderSrc = R"(#version 300 es 
    void main(){}
)";

const char* kDepthVertexShaderSrc = R"(#version 300 es
    in vec3 position;

    uniform mat4 cameraMatrix;

    void main()
    {
        gl_Position = cameraMatrix * vec4(position, 1.0);
    }
)";