#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    int haveDiffSampler;
    int haveSpecSampler;
    int diffCount;
    int specCount;
    sampler2D diffMap;
    sampler2D specMap;
};
uniform Material material;

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
out vec4 FragColor;

void main()
{   
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    //漫反射分量
    float diff = max(dot(lightDir,norm), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    if (material.haveDiffSampler == 1)
    {
        diffuse *= vec3(texture(material.diffMap, TexCoords));
        ambient *= vec3(texture(material.diffMap, TexCoords));
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //镜面反射分量
    float spec = pow( max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    if (material.haveSpecSampler == 1)
        specular *= vec3(texture(material.specMap, TexCoords));

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}