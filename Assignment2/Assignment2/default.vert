#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal; 
uniform mat4 MVP;
uniform mat4 model;

out vec3 Normal;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0f);
    Normal = normalize(mat3(model) * normal);
}