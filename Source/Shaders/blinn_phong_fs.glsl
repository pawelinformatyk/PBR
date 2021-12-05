#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 Albedo;
uniform float Metallic;
uniform float Roughness;
uniform float AO;

#define MAX_LIGHTS 256

uniform int LightsNum;
uniform vec3 LightPositions[MAX_LIGHTS];

uniform vec3 CameraPos;


void main()
{		
    //vec3 Diffuse = Albedo;
    vec3 Diffuse = pow(Albedo, vec3(2.2));
    float Specular = Metallic;
    //float Roughness = 1 - Roughness;
    //float Shininess = (2 / (Roughness * Roughness) - 2) * 10;

    vec3 N = normalize(Normal);

    vec3 DiffusePart = vec3(0.);
    vec3 SpecularPart = vec3(0.);

    vec3 viewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        vec3 LightDir = normalize(LightPositions[i] - WorldPos);
        
        float Diff = max(dot(LightDir, N), 0.0);

        vec3 HalfwayDir = normalize(LightDir + viewDir);  
        float Spec = pow(max(dot(N, HalfwayDir), 0.0), 32);

        float Distance = length(LightPositions[i] - WorldPos);  
        float Attenuation = 300. / ( Distance * Distance);    
    
        DiffusePart  += vec3(1.) * Diff * Diffuse * Attenuation;
        SpecularPart += vec3(0.8) * Spec * Specular * Attenuation; 
    }
    
    vec3 Ambient = vec3(0.03) * Diffuse * AO;
    vec3 Color = Ambient + DiffusePart + SpecularPart;

    FragColor = vec4(Color, 1.0);
}