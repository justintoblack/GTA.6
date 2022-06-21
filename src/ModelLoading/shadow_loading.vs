#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float ax = (100 * aPos.x - 100 * aPos.y) / (100 - aPos.y);
    float az = (100 * aPos.z - 100 * aPos.y) / (100 - aPos.y);
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(ax, 0.0, az, 1.0);
}