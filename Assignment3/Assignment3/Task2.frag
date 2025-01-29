#version 330 core

uniform sampler2D myTexture;
uniform vec3 lightPos;
uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform float kc;
uniform float kl;
uniform float kq;
 
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
 
out vec4 FragColor;
 
void main()
{
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (kc + kl * distance + kq * distance * distance);
 
	vec3 ambient = ambientColor * texture(myTexture,TexCoord).rgb;
 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
 
	float diff = max(dot(norm,lightDir), 0.0);
	vec3 diffuse = diff * diffuseColor * texture(myTexture, TexCoord).rgb * attenuation;
 
	vec3 result = ambient + diffuse;
	FragColor = vec4(result, 1.0); 
}