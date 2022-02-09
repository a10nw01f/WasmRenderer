R"(#version 300 es

out vec2 fragUV;

const vec2 verts[4] = vec2[](
    vec2(-1.0,-1.0),
    vec2(-1.0,1.0),
    vec2(1.0,-1.0),
    vec2(1.0,1.0)
);

void main()
{
    vec2 pos = verts[gl_VertexID];
    gl_Position = vec4(pos, 0.0, 1.0);
    fragUV = pos * 0.5 + vec2(0.5);
}

)"