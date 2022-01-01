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


void main()
{		
    vec3 Diffuse = pow(Albedo, vec3(2.5));
    // Transfer of PBR parameters. 
    float Specular = 1 - Roughness;
    float Shininess = 20. / (Roughness * Roughness) - 2.;

    vec3 N = normalize(Normal);

    vec3 DiffusePart = vec3(0.);
    vec3 SpecularPart = vec3(0.);

    vec3 viewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        vec3 LightDir = normalize(LightPositions[i] - WorldPos);
        
        float Diff = max(dot(LightDir, N), 0.0);

        vec3 HalfwayDir = normalize(LightDir + viewDir);  
        float Spec = pow(max(dot(N, HalfwayDir), 0.0), Shininess);

        float Distance = length(LightPositions[i] - WorldPos);  
        float Attenuation = 375. / ( Distance * Distance);    
    
        DiffusePart  += vec3(1.) * Diff * Diffuse * Attenuation;
        SpecularPart += vec3(0.8) * Spec * Specular * Attenuation; 
    }
    
    vec3 Ambient = vec3(0.03) * Diffuse;

    vec3 Color = Ambient + DiffusePart + SpecularPart;

    // Output color of pixel. 
    FragColor = vec4(Color, 1.0);
}