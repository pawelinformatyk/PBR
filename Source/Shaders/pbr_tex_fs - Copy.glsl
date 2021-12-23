#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D AlbedoMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;

#define MAX_LIGHTS 992

uniform int LightsNum;
uniform vec3 LightPositions[MAX_LIGHTS];

uniform vec3 CameraPos;


vec3 GetNormalFromMap()
{
    vec3 TangentNormal = texture(NormalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 St1 = dFdx(TexCoords);
    vec2 St2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*St2.t - Q2*St1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * TangentNormal);
}

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float Roughness)
{
    float A2 = Roughness*Roughness * Roughness*Roughness;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float Nom   = A2;
    float Denom = (NdotH2 * (A2 - 1.0) + 1.0);
    Denom = PI * Denom * Denom;

    return Nom / Denom;
}

float GeometrySchlickGGX(float NdotV, float Roughness)
{
    float r = (Roughness + 1.0);
    float k = (r*r) / 8.0;

    float Nom   = NdotV;
    float Denom = NdotV * (1.0 - k) + k;

    return Nom / Denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float Ggx2 = GeometrySchlickGGX(NdotV, Roughness);
    float Ggx1 = GeometrySchlickGGX(NdotL, Roughness);

    return Ggx1 * Ggx2;
}

vec3 FresnelSchlick(float CosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    vec3 Albedo     = pow(texture(AlbedoMap, TexCoords).rgb, vec3(2.2));
    float Metallic  = texture(MetallicMap, TexCoords).r;
    float Roughness = texture(RoughnessMap, TexCoords).r;

    vec3 N = GetNormalFromMap();
    vec3 V = normalize(CameraPos - WorldPos);

    // Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the Albedo Color as F0 (Metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, Albedo, Metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        // Calculate per-light Radiance
        vec3 L = normalize(LightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float Distance = length(LightPositions[i] - WorldPos);
        float Attenuation = 1.0 / (Distance * Distance);
        vec3 Radiance = vec3(300.0) * Attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);   
        float G   = GeometrySmith(N, V, L, Roughness);      
        vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 Numerator    = NDF * G * F; 
        float Denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 Specular = Numerator / Denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // For energy conservation, the Diffuse and Specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the Diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // Multiply kD by the inverse metalness such that only non-metals 
        // have Diffuse lighting, or a linear blend if partly metal (pure metals
        // have no Diffuse light).
        kD *= 1.0 - Metallic;	  

        // Scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // Add to outgoing Radiance Lo
        Lo += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }   
    
    vec3 Ambient = vec3(0.03) * Albedo;

    vec3 Color = Ambient + Lo;

    // HDR tonemapping
    Color = Color / (Color + vec3(1.0));

    // Gamma correct
    Color = pow(Color, vec3(1.0/2.2)); 

    FragColor = vec4(Color, 1.0);
}