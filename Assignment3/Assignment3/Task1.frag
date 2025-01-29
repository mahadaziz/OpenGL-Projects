#version 330 core
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D myTexture;

void main()
{
	float c = dot(Normal, vec3(0.8,0.7,0.6));
	FragColor = texture(myTexture, TexCoord);
}