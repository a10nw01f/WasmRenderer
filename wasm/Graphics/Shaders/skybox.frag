R"(#version 300 es

precision mediump float;

in vec3 fragPos;

out vec4 outColor;

uniform samplerCube skybox;

uniform float exposure;

void main()
{
    vec3 resultColor = texture(skybox, fragPos).xyz * exposure;
    outColor.xyz = resultColor; // (resultColor + vec3(1.0));
    outColor.w = 1.0;
}
)"