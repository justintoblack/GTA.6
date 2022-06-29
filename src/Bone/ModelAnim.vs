#version 330 core
//precision mediump float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

const int MAX_BONE_INFLUENCE = 4;
const int MAX_BONES = 250;
const int DEFAULT_BONE_ID = -1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos =vec3(model * vec4(aPos,1.0f));   

    vec4 position = vec4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIDs[i] == DEFAULT_BONE_ID)
            continue;
        if (boneIDs[i] >= MAX_BONES)
        {
            position = vec4(aPos, 1.0f);
            break;
        }
        position += finalBonesMatrices[boneIDs[i]] * weights[i] * vec4(aPos, 1.0f);
    }
    gl_Position = projection * view * model * position;
}