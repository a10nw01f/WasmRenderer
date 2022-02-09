R"(#version 300 es
    precision mediump float;

    in vec2 fragUV;
    in vec3 fragNormal;
    in vec3 fragPos;

    out vec4 outColor;

    uniform sampler2D albedoMap;
    uniform sampler2D roughnessMap;
    uniform sampler2D metallicMap;
    uniform sampler2D normalMap;

    uniform samplerCube prefilteredMap;
    uniform sampler2D brdfLutMap;

    uniform bool useIBL;
    uniform vec3 cameraPos;
    uniform float exposure;

    uniform float metallic;
    uniform float roughness;

    uniform uint pointLightCount;

    struct PointLight
    {
        vec3 position;
        vec3 color;
    };

    layout (std140) uniform PointLights{
        PointLight pointLights[32];
    };

    uniform uint directionalLightCount;

    struct DirectionalLight
    {
        vec3 direction;
        vec3 color;
    };

    layout (std140) uniform DirectionalLights{
        DirectionalLight directionalLights[32];
    };

    #define PI 3.141592653589793
    #define GAMMA 2.2

    vec3 FresnelSchlickRoughness(float cosTheta, vec3 f0, float roughness)
    {
        return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - cosTheta, 5.0);
    } 

    vec3 IblDiffuse(vec3 albedo, vec3 n)
    {
        vec3 diffuseColor = textureLod(prefilteredMap, n, 7.6).xyz * exposure;
        return albedo * diffuseColor;
    }

    float SpecularDistribution(float roughness, float NoH)
    {
        float rough2 = roughness * roughness;
        float rough4 = rough2 * rough2;

        float d = (NoH * rough4 - NoH) * NoH + 1.0;
	    return rough4 / (PI * (d * d));
    }

    float SpecularGeometry(float roughness, float NoL, float NoV)
    {
        float k = roughness;
	    float GSmithL = NoL * (1.0 - k) + k;
	    float GSmithV = NoV * (1.0 - k) + k;
        return 0.5 / (GSmithL * GSmithV);
    }

    vec3 SpecularFresnel(vec3 f0, float VoH)
    {
        return f0 + (vec3(1.0) - f0) * pow(1.0 - VoH, 5.0);
        //exp2(((-5.55473 * VoH) - 6.98316) * VoH);
    }

    vec3 IblSpecular(vec3 v, vec3 r, vec3 n, vec3 f0, vec3 albedo, float metallic, float roughness)
    {
        float NoV = max(dot(n, v), 0.0);

        const float kMipLevelCount = 9.0;
	    float lod = roughness * kMipLevelCount;
        vec3 prefilteredColor = textureLod(prefilteredMap, r, lod).xyz * exposure;

        vec2 integrateBrdf = texture(brdfLutMap, vec2(NoV, roughness)).xy;
        vec3 f = FresnelSchlickRoughness(NoV, f0, roughness);

        return prefilteredColor * (f * integrateBrdf.x + integrateBrdf.y);
    }

    vec3 Diffuse(vec3 albedo, vec3 lightColor)
    {
        return albedo * lightColor;
    }

    // http://www.thetenthplanet.de/archives/1180
    mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
    {
        // get edge vectors of the pixel triangle
        vec3 dp1 = dFdx( p );
        vec3 dp2 = dFdy( p );
        vec2 duv1 = dFdx( uv );
        vec2 duv2 = dFdy( uv );
    
        // solve the linear system
        vec3 dp2perp = cross( dp2, N );
        vec3 dp1perp = cross( N, dp1 );
        vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
        vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    
        // construct a scale-invariant frame 
        float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
        return mat3( T * invmax, B * invmax, N );
    }

    vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord, vec3 map )
    {
        map = map * 255./127. - 128./127.;
        mat3 TBN = cotangent_frame(N, -V, texcoord);
        return normalize(TBN * map);
    }

    
    vec3 ApplyLight(float metallic, float roughness, vec3 albedo, vec3 f0, vec3 l, vec3 v, vec3 n)
    {
        roughness = max(roughness, 2.0e-3);
        vec3 h = normalize(l + v);
        float NoL = max(dot(n,l), 0.0);
        float NoH = max(dot(n,h), 0.0);
        float VoH = max(dot(v,h), 0.0);
        float NoV = max(dot(n,v), 0.0);

        float sD = SpecularDistribution(roughness, NoH);
        float sG = SpecularGeometry(roughness, NoL, NoV);
        vec3 sF = SpecularFresnel(f0,VoH);

        vec3 specular = sD * sG * sF;

        vec3 kS = sF;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        return (kD * albedo / PI + specular) * NoL;
    }
    
    vec3 ApplyPointLight(PointLight light, float metallic, float roughness, vec3 albedo, vec3 f0, vec3 v, vec3 n)
    {
        vec3 l = light.position - fragPos;
        float distanceSqr = dot(l,l);
        float distance = sqrt(distanceSqr);
        l /= distance;

        float attenuation = 1.0 / distanceSqr;
        vec3 radiance = light.color * attenuation;

        return radiance * ApplyLight(metallic, roughness, albedo, f0, l, v, n);
    }

    vec3 ApplyDirectionalLight(DirectionalLight light, float metallic, float roughness, vec3 albedo, vec3 f0, vec3 v, vec3 n)
    {
        return light.color * ApplyLight(metallic, roughness, albedo, f0, -light.direction, v, n);
    }

    void main()
    {
            vec3 v = normalize(cameraPos - fragPos);
            vec3 normalMapValue = texture(normalMap, fragUV ).xyz;
            vec3 n = fragNormal;
            if(normalMapValue.x + normalMapValue.y + normalMapValue.z < 2.99)
            {
                n = perturb_normal(fragNormal, fragPos, fragUV, normalMapValue);
            }
            
            vec4 albedo = texture(albedoMap, fragUV);
            albedo.rgb = pow(albedo.rgb, vec3(GAMMA));
            float roughnessValue = texture(roughnessMap, fragUV).x * roughness;
            float metallicValue = texture(metallicMap, fragUV).x * metallic;

            vec3 f0 = mix(vec3(0.04), albedo.xyz, metallicValue);
            vec3 resultColor = vec3(0.0);

            if(useIBL)
            {
                vec3 r = reflect(-v, n);
                vec3 diffuse = IblDiffuse(albedo.xyz, n);
                vec3 specular = IblSpecular(v, r, n, f0, albedo.xyz, metallicValue, roughnessValue);
                resultColor = diffuse + specular;
            }

            for(uint i = 0u; i < pointLightCount; i++)
            {   
                resultColor += ApplyPointLight(pointLights[i], metallicValue, roughnessValue, albedo.xyz, f0, v, n);
            }

            for(uint i = 0u; i < directionalLightCount; i++)
            {   
                resultColor += ApplyDirectionalLight(directionalLights[i], metallicValue, roughnessValue, albedo.xyz, f0, v, n);
            }

            outColor = vec4(resultColor, albedo.w);
            outColor.rgb = pow(outColor.rgb, vec3(1.0/GAMMA));
    }
)"