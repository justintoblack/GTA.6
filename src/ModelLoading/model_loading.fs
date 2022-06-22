#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;
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
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
out vec4 FragColor;

void main()
{   
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    //���������
    vec3 norm = normalize(Normal);
    //lightDirָ���ǣ���Ƭ��ָ���Դ
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir,norm), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    diffuse += light.diffuse * diff * texture(material.texture_diffuse2, TexCoords).rgb;

     //���淴�����
    vec3 viewDir = normalize(viewPos - FragPos);
    // �������
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 reflectDir = reflect(-lightDir, norm);
    //ԭ�������߷���ͷ��䷽��ĵ�˼���н�
    //float spec = pow( max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //���ڸ�Ϊ���淨�ߺͰ����������н�
    float spec = pow( max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = vec3(0.0f);
    if (material.specularCount >= 1)
    {
        specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;
        if (material.specularCount >= 2)
        specular += light.specular * spec * texture(material.texture_specular2, TexCoords).rgb;
    }
    else
    {
        specular = light.specular * spec * vec3(1.0f);
    }
    
    //emission
    //vec3 emission = texture(material.texture_emissive, TexCoords).rgb;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}