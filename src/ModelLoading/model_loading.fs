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
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quardratic;
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

vec3 CalcDirLight(Light light, vec3 normal, bool useAttenuation);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal);


void main()
{   
    vec3 result = CalcDirLight(light, Normal, false);
    if (useSpotLight == 1)
        result += CalcSpotLight(spotLight, Normal);
    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(Light dirLight, vec3 normal, bool useAttenuation)
{
    vec3 ambient = dirLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    //���������
    vec3 norm = normalize(normal);
    //lightDirָ���ǣ���Ƭ��ָ���Դ
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(lightDir,norm), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    diffuse += dirLight.diffuse * diff * texture(material.texture_diffuse2, TexCoords).rgb;

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

    // attenuation ˥��
    if (useAttenuation){
    //lightDir,Ƭ��ָ���Դ
        vec3 lightDir = normalize(dirLight.position - FragPos);
        float distance = length(dirLight.position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.07 * distance + 0.017 * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        float theta = dot(-lightDir, normalize(dirLight.direction));
        //����Բ׶���ҵĲ�
        float epsilon = (dirLight.cutOff - dirLight.outerCutOff);
        //I = (theta - outerCutOff) / (cutOff - outerCutOff), ��clamp������0.0��1.0��Χ
        float intensity = clamp((theta - dirLight.outerCutOff)/epsilon, 0.0, 1.0 );
        ambient *= intensity;
        diffuse *= intensity;
        specular *= intensity;
        ambient = vec3(0.0);
    }

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal)
{
    //lightDir,Ƭ��ָ���Դ
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float theta = dot(-lightDir, normalize(spotLight.direction));

    //if (theta > spotLight.cutOff)
    //{
    //    Light tmpLight;
    //    tmpLight.position = spotLight.position;
    //    tmpLight.direction = spotLight.direction;
    //    tmpLight.ambient = spotLight.ambient;
    //    tmpLight.diffuse = spotLight.diffuse;
    //    tmpLight.specular = spotLight.specular;
    //    tmpLight.constant = spotLight.constant;
    //    tmpLight.linear = spotLight.linear;
    //    tmpLight.quardratic = spotLight.quardratic;
    //    return CalcDirLight(tmpLight, normal, true);
    //}
    //else{
    //    return vec3(0.0);
    //}

    Light tmpLight;
    tmpLight.position = spotLight.position;
    tmpLight.direction = spotLight.direction;
    tmpLight.ambient = spotLight.ambient;
    tmpLight.diffuse = spotLight.diffuse;
    tmpLight.specular = spotLight.specular;
    tmpLight.constant = spotLight.constant;
    tmpLight.linear = spotLight.linear;
    tmpLight.quardratic = spotLight.quardratic;
    tmpLight.cutOff = spotLight.cutOff;
    tmpLight.outerCutOff = spotLight.outerCutOff;
    return CalcDirLight(tmpLight, normal, true);
    

}