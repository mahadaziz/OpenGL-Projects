#version 330 core

uniform sampler2D myTexture;
uniform vec3 lightPos;
uniform vec3 diffuseColor1;
uniform vec3 diffuseColor2;
uniform vec3 diffuseColor3;
uniform vec3 ambientColor;
uniform vec3 spotDirection1;
uniform vec3 spotDirection2;
uniform vec3 spotDirection3;
uniform float kc;
uniform float kl;
uniform float kq;
uniform float cutoff;
 
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
 
out vec4 FragColor;
 
void main()
{
	vec3 norm = normalize(Normal);
	vec3 result = vec3(0);

	//Spotlight 1
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (kc + kl * distance + kq * distance * distance);

	vec3 lightDir = normalize(lightPos - FragPos);
	float theta = dot(lightDir, normalize(-spotDirection1));
	float epsilon = cutoff + 0;
    float intensity = clamp((theta - cutoff) / (epsilon - cutoff), 0.0, 1.0);


	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseColor1 * diff * texture(myTexture, TexCoord).rgb * attenuation * intensity;

	vec3 ambient = ambientColor * texture(myTexture,TexCoord).rgb;

	result += ambient + diffuse;

	//Spotlight 2
	lightDir = normalize(lightPos - FragPos);
	theta = dot(lightDir, normalize(-spotDirection2));
	epsilon = cutoff + 0;
    intensity = clamp((theta - cutoff) / (epsilon - cutoff), 0.0, 1.0);


	diff = max(dot(norm, lightDir), 0.0);
    diffuse = diffuseColor2 * diff * texture(myTexture, TexCoord).rgb * attenuation * intensity;

	ambient = ambientColor * texture(myTexture,TexCoord).rgb;

	result += ambient + diffuse;

	//Spotlight 3
	lightDir = normalize(lightPos - FragPos);
	theta = dot(lightDir, normalize(-spotDirection3));
	epsilon = cutoff + 0;
    intensity = clamp((theta - cutoff) / (epsilon - cutoff), 0.0, 1.0);

	diff = max(dot(norm, lightDir), 0.0);
    diffuse = diffuseColor3 * diff * texture(myTexture, TexCoord).rgb * attenuation * intensity;

	ambient = ambientColor * texture(myTexture,TexCoord).rgb;

	result += ambient + diffuse;

	FragColor = vec4(result, 1.0);
 
}