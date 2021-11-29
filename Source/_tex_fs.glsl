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
uniform vec3 lightPositions[4];

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
    vec3 color = texture(AlbedoMap, TexCoords).rgb;
    
    vec3 N = getNormalFromMap();

    vec3 ambient = 0.05 * color;
    vec3 diffuse = vec3(0.);
    vec3 specular = vec3(0.);

    vec3 viewDir = normalize(CameraPos - WorldPos);

    for(int i = 0; i < 4; ++i) 
    {
        vec3 lightDir = normalize(lightPositions[i] - WorldPos);
        
        float diff = max(dot(lightDir, N), 0.0);

        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(N, halfwayDir), 0.0), 32.0);

        float distance = length(lightPositions[i] - WorldPos);  
        float attenuation = 4.0 / ( distance);    
    
        specular += vec3(1) * spec * attenuation; 
        diffuse += diff * color * attenuation;
    }   
    
    // specular
    FragColor = vec4( ambient + diffuse + specular, 1.0);
}