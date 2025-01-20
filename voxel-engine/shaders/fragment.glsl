#version 330 core
struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
};

out vec4 FragColor;

in vec3 ourColor;
in vec3 Normal;

uniform vec3 viewPos;
uniform Light light;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse;
        
    vec3 result = (ambient + diffuse) * ourColor;
    FragColor = vec4(result, 1.0);
} 