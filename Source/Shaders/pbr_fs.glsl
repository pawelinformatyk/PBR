#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 Albedo;
uniform float Metallic;
uniform float Roughness;

#define MAX_LIGHTS 992

uniform int LightsNum;
uniform vec3 LightPositions[MAX_LIGHTS];

uniform vec3 CameraPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float Roughness)
{
    float a = Roughness*Roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float Nom   = a2;
    float Denom = (NdotH2 * (a2 - 1.0) + 1.0);
    Denom = PI * Denom * Denom;

    return Nom / Denom;
}

float GGX(float NdotV, float Roughness)
{
    float a = Roughness*Roughness;
    float a2 = a*a;

    float Nom   = 2 * NdotV;
    float Denom = NdotV + sqrt(a2 + (1-a2) * NdotV * NdotV);

    return Nom / Denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float Ggx2 = GGX(NdotV, Roughness);
    float Ggx1 = GGX(NdotL, Roughness);

    return Ggx1 * Ggx2;
}

vec3 FresnelCookTorrence(float CosTheta, vec3 F0)
{
    F0 = sqrt(F0);
    vec3 n = (1. + F0) / (1. - F0);
    float c = CosTheta;
    vec3 g = sqrt(n*n + c*c - 1.);
    
    vec3 expr1 = (g-c) / (g + c);
    expr1 *= expr1;

    vec3 expr2 = ((g + c) * c - 1.)/((g - c) * c + 1.);
    expr2 *= expr2;

    return 0.5 * expr1 * (1. + expr2);
}

void main()
{		
    vec3 N = normalize(Normal);
    vec3 V = normalize(CameraPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, Albedo, Metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        float Distance = length(LightPositions[i] - WorldPos);
        float Attenuation = 1.0 / (Distance * Distance);
        vec3 Radiance = vec3(300.0) * Attenuation;

        vec3 L = normalize(LightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);   
        float G   = GeometrySmith(N, V, L, Roughness);      
        vec3 F    = FresnelCookTorrence(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 Numerator    = NDF * G * F; 
        float Denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 Specular = Numerator / (Denominator + 0.0001);
        
        vec3 kS = F;
        // Energy conservation, 
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;	  

        float NdotL = max(dot(N, L), 0.0);        

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