#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D AlbedoMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D AOMap;

// lights
#define MAX_LIGHTS 256

uniform int LightsNum = 4;
uniform vec3 LightPositions[MAX_LIGHTS];

uniform vec3 CameraPos;


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(NormalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{		
    vec3 Diffuse = texture(AlbedoMap, TexCoords).rgb;

    vec3 N = getNormalFromMap();

    vec3 ambient = vec3(0.);
    vec3 diffuse = vec3(0.);
    vec3 specular = vec3(0.);

    vec3 viewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < min(MAX_LIGHTS,LightsNum); ++i) 
    {
        vec3 lightDir = normalize(LightPositions[i] - WorldPos);
        
        float diff = max(dot(lightDir, N), 0.0);

        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(N, halfwayDir), 0.0), 16.);

        float distance = length(LightPositions[i] - WorldPos);  
        float attenuation = 300.0 / ( distance * distance);    
    
        ambient  += vec3(0.03) * Diffuse * attenuation;
        diffuse  += vec3(0.8) * diff      * Diffuse * attenuation;
        specular += vec3(1) * spec * attenuation; 
    }
    
    vec3 Color = ambient+diffuse+specular;
    
    // HDR 
    Color = Color / (Color + vec3(1.0));

    // Gamma
    Color = pow(Color, vec3(1.0/2.2)); 

    FragColor = vec4(Color, 1.0);
}