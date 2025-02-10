#version 330 core

in vec2 TexCoord;    // Texture coordinates from the vertex shader
in vec3 FragPos;     // Fragment position in world space
in vec3 Normal;      // Normal vector in world space

out vec4 FragColor;  // Final fragment color

// Uniforms for the lighting

struct Material {
    sampler2D texture1;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * material.ambient;  // Basic ambient lighting with the material's ambient color

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);  // Lambertian reflection
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Combine the results
    vec3 result = ambient + diffuse + specular;

    // Apply the texture
    vec4 textureColor = texture(material.texture1, TexCoord);

    // Final color: blend the lighting result with the texture
    FragColor = textureColor * vec4(result, 1.0);
}
