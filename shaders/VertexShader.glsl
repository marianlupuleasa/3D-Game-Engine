#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aNormal;   // Vertex normal
layout(location = 2) in vec2 aTexCoord; // Texture coordinates

out vec2 TexCoord;   // Texture coordinates
out vec3 FragPos;    // Fragment position in world space
out vec3 Normal;     // Normal vector to be used in the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0f)); // Convert position to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Transform the normal to world space
    TexCoord = aTexCoord;  // Pass texture coordinates to the fragment shader

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
