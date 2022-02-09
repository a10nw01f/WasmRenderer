R"(#version 300 es

precision mediump float;
precision mediump int;

in vec3 fragPos;

out vec4 outColor;

uniform samplerCube envMap;
uniform float roughness;
uniform float resolution;

#define PI 3.141592653589793

vec2 Hammersley(uint i, uint max);
vec3 ImportaceSampleGGX(vec2 xi, float roughness, vec3 n);
float DistributionGGX(vec3 N, vec3 H, float roughness);

void main()
{
    vec3 n = normalize(fragPos);
    vec3 v = n;
    vec3 r = n;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    const uint kNumSamples = 256u;
	
    for(uint i = 0u; i < kNumSamples; i++)
    {
        vec2 xi = Hammersley(i, kNumSamples);
        vec3 h = ImportaceSampleGGX(xi, roughness, n);
    
        vec3 l = 2.0 * dot(v,h) * h - v;
        float NoL =  max(dot(n,l), 0.0);
        if(NoL > 0.0)
        {
            float NoH = max(dot(n, h), 0.0);
            float HoV = max(dot(h,v), 0.0);
            float D   = DistributionGGX(n, h, roughness);
            float pdf = (D * NoH / (4.0 * HoV)) + 0.0001; 

            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(kNumSamples) * pdf + 0.0001);
            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            prefilteredColor += textureLod(envMap, l, mipLevel).xyz * NoL;
            totalWeight += NoL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    outColor = vec4(prefilteredColor, 1.0);
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
)"