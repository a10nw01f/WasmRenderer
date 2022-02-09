R"(#version 300 es

precision mediump float;
precision mediump int;

in vec2 fragUV;

out vec4 outColor;

#define PI 3.141592653589793

vec2 Hammersley(uint xi, uint count);
vec3 ImportaceSampleGGX(vec2 xi, float roughness, vec3 n);
float GeometrySmith(float roughness, float NoV,  float NoL);

vec2 IntegrateBrdf(float NoV, float roughness)
{
    float a = 0.0;
    float b = 0.0;

    vec3 v;
    v.x = sqrt(1.0 - NoV * NoV);
    v.y = 0.0;
    v.z = NoV;

    vec3 n = vec3(0.0,0.0,1.0);

    const uint kSampleCount = 1024u;
    for(uint i = 0u; i < kSampleCount; i++)
    {
        vec2 xi = Hammersley(i, kSampleCount);
        vec3 h = ImportaceSampleGGX(xi, roughness, n);
        vec3 l = 2.0 * dot(v,h) * h - v;

        float NoL = max(dot(n,l),0.0);
        float NoH = max(dot(n,h),0.0);
        float VoH = max(dot(v,h),0.0);

        if(NoL > 0.0)
        {
            float g = GeometrySmith(roughness, NoV, NoL);
            float gVis = g * VoH / (NoH * NoV);
            float fc = pow(1.0 - VoH, 5.0);
            
            a += (1.0 - fc) * gVis;
            b += fc * gVis;
        }
    }

    a /= float(kSampleCount);
    b /= float(kSampleCount);

    return vec2(a,b);
}

void main()
{
    outColor = vec4(IntegrateBrdf(fragUV.x, fragUV.y), 0.0, 1.0);
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint n)
{
    return vec2(float(i)/float(n), RadicalInverse_VdC(i));
} 

vec3 ImportaceSampleGGX(vec2 xi, float roughness, vec3 n)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 h;
    h.x = sinTheta * cos(phi);
    h.y = sinTheta * sin(phi);
    h.z = cosTheta;

    vec3 up = abs(n.z) < 0.999 ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
    vec3 tangentX = normalize(cross(up, n));
    vec3 tangentY = cross(n, tangentX);

    return tangentX * h.x + tangentY * h.y + n * h.z;
}

float GeometrySchlickGGX(float cosTheta, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = cosTheta;
    float denom = cosTheta * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float roughness, float NoV,  float NoL)
{
    float ggx1 = GeometrySchlickGGX(NoL, roughness);
    float ggx2 = GeometrySchlickGGX(NoV, roughness);

    return ggx1 * ggx2;
}


)"