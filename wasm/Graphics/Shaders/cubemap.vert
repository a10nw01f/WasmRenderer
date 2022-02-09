R"(#version 300 es
in vec3 inPos;

out vec3 fragPos;

uniform mat4 cameraMatrix;

void main()
{
    gl_Position = cameraMatrix * vec4(inPos, 1.0);
    fragPos = inPos;
})"