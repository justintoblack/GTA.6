#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Material
{
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_emissive;
    int diffuseCount;
    int specularCount;
};
uniform Material material;

struct Light{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct SpotLight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quardratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;
uniform int useSpotLight;
out vec4 FragColor;

vec3 CalcDirLight(Light light, vec3 normal);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal);


void main()
{   
    vec3 result = CalcDirLight(light, Normal);
    if (useSpotLight == 1)
        result += CalcSpotLight(spotLight, Normal);
    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(Light dirLight, vec3 normal)
{
    vec3 ambient = dirLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    //漫反射分量
    vec3 norm = normalize(normal);
    //lightDir指的是：从片段指向光源
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(lightDir,norm), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    diffuse += dirLight.diffuse * diff * texture(material.texture_diffuse2, TexCoords).rgb;

     //镜面反射分量
    vec3 viewDir = normalize(viewPos - FragPos);
    // 半程向量
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);
    //原来是视线方向和反射方向的点乘计算夹角
    //float spec = pow( max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //现在改为表面法线和半程向量计算夹角
    float spec = pow( max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = vec3(0.0f);
    if (material.specularCount >= 1)
    {
        specular = dirLight.specular * spec * texture(material.texture_specular1, TexCoords).rgb;
        if (material.specularCount >= 2)
        specular += dirLight.specular * spec * texture(material.texture_specular2, TexCoords).rgb;
    }
    else
    {
        specular = dirLight.specular * spec * vec3(1.0f);
    }
    
    //emission
    //vec3 emission = texture(material.texture_emissive, TexCoords).rgb;
    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal)
{
    //lightDir,片段指向光源
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float theta = dot(-lightDir, normalize(spotLight.direction));
    if (theta > spotLight.cutOff)
    {
        Light tmpLight;
        tmpLight.position = spotLight.position;
        tmpLight.direction = spotLight.direction;
        tmpLight.ambient = spotLight.ambient;
        tmpLight.diffuse = spotLight.diffuse;
        tmpLight.specular = spotLight.specular;

        return CalcDirLight(tmpLight, normal);
    }
    else{
        return vec3(0.0);
    }
}