R"(#version 300 es
    in vec3 inPosition;
    in vec2 inUV;
    in vec3 inNormal;

    out vec2 fragUV;
    out vec3 fragNormal;
    out vec3 fragPos;

    uniform mat4 cameraMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 modelMatrix;

    void main()
    {
        gl_Position = cameraMatrix * vec4(inPosition, 1.0);
        fragUV = inUV;
        fragPos = (modelMatrix * vec4(inPosition, 1.0)).xyz;
        fragNormal = normalize((modelMatrix * vec4(inNormal, 0.0)).xyz);
    }
)"