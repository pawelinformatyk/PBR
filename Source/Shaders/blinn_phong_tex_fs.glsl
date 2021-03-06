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

void main()
{		
    vec3 Diffuse = texture(AlbedoMap, TexCoords).rgb;
    // Transfer of PBR parameters. 
    float Roughness = texture(RoughnessMap, TexCoords).r;
    float Specular = 1 - Roughness;
    float Shininess = 12.5 / (Roughness * Roughness) - 2.;

    vec3 N = GetNormalFromMap();

    vec3 DiffusePart = vec3(0.);
    vec3 SpecularPart = vec3(0.);

    vec3 ViewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        vec3 LightDir = normalize(LightPositions[i] - WorldPos);
        
        float Diff = max(dot(LightDir, N), 0.0);

        vec3 HalfwayDir = normalize(LightDir + ViewDir);  
        float Spec = pow(max(dot(N, HalfwayDir), 0.0), Shininess);

        float Distance = length(LightPositions[i] - WorldPos);  
        float Attenuation = 300. / ( Distance * Distance);    
    
        DiffusePart  += vec3(0.3) * Diff * Diffuse * Attenuation;
        SpecularPart += vec3(0.8) * Spec * Specular * Attenuation; 
    }
    
    vec3 Ambient = vec3(0.2) * Diffuse;

    vec3 Color = Ambient + DiffusePart + SpecularPart;
    
    FragColor = vec4(Color, 1.0);
}