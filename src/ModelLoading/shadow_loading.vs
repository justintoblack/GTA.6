#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec3 light;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //float ax = (100 * aPos.x - 100 * aPos.y) / (100 - aPos.y);
    //float az = (100 * aPos.z - 100 * aPos.y) / (100 - aPos.y);

    vec4 mideum = model * vec4(aPos, 1.0);
    float ax = (light.y * mideum.x - light.x * mideum.y) / (light.y - mideum.y);
    float az = (light.y * mideum.z - light.x * mideum.y) / (light.y - mideum.y);
    vec4 last = vec4(ax, 0.05, az, mideum.w);

    TexCoords = aTexCoords;    
  
    gl_Position = projection * view * last;
}