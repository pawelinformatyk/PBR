#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 FragColor;

uniform vec3 Albedo;
uniform float Metallic;
uniform float Roughness;

#define MAX_LIGHTS 992

uniform int LightsNum;
uniform vec3 LightPositions[MAX_LIGHTS];

uniform vec3 CameraPos;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;


void main()
{
    gl_Position =  Projection * View * Model * vec4(aPos, 1.0);

    // Vertex position. 
    vec3 WorldPos = vec3(Model * vec4(aPos,1.0));

    vec3 Diffuse = pow(Albedo, vec3(2.5));
    // Transfer of PBR parameters. 
    float Specular = 1 - Roughness;
    float Shininess = 20. / (Roughness * Roughness) - 2.;

    vec3 N = normalize(aNormal);

    vec3 DiffusePart = vec3(0.);
    vec3 SpecularPart = vec3(0.);

    vec3 ViewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        vec3 LightDir = normalize(LightPositions[i] - WorldPos);
        
        float Diff = max(dot(LightDir, N), 0.0);

    	vec3 ReflectDir = reflect(-LightDir, N);
        float Spec = pow(max(dot(ViewDir,ReflectDir), 0.0), Shininess);

        float Distance = length(LightPositions[i] - WorldPos);  
        float Attenuation = 300. / ( Distance * Distance);    
    
        DiffusePart  += vec3(1.45) * Diff * Diffuse * Attenuation;
        SpecularPart += vec3(.8) * Spec * Specular * Attenuation; 
    }
    
    vec3 Ambient = vec3(0.83) * Diffuse;

    vec3 Color = Ambient + DiffusePart + SpecularPart;

    // Output color passed to Fragment Shader. 
    FragColor = vec4(Color, 1.0);
}