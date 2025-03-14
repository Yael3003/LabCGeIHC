#version 330 core

in vec3 our_normal;
in vec3 fragPos;
out vec4 color;

struct PositionalLight{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material{
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		float shininess;
};

uniform vec3 viewPos; 
uniform PositionalLight light;
uniform Material material;

void main(){

	// Iluminacion ambiental
	vec3 ambient = light.ambient * material.ambient;
	// Iluminacion difusa
	vec3 lightDir = normalize(fragPos - light.position);
	float diff = max(dot(our_normal, -lightDir), 0.0);
	vec3 diffuse  = diff * light.diffuse * material.diffuse;
	// Iluminacion specular
	vec3 r = reflect(lightDir, our_normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	float spec = pow( max(dot(r , viewDir), 0.0), material.shininess);
	vec3 specular  = spec * light.specular * material.specular;

	vec3 result = ambient + diffuse + specular;

	color = vec4(result, 1.0);

}
