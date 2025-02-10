#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <mat4.h>
#include <vec3.h>
#include <vec2.h>

// Vertex data structure
struct Vertex 
{
    Vec3 position;
    Vec3 normal;
    Vec2 texture;
};

// Material data structure
struct Material 
{
    unsigned int texture;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
};

struct Light {
    Vec3 position;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
};

// Mesh class
class Mesh 
{
public:
    unsigned int vao, vbo, ebo, textureID;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    Light light;

	// Constructor
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material material, const Light light, unsigned int textureID) : material(material), light(light), textureID(textureID) 
    {

        this->vertices = vertices;
        this->indices = indices;

        // Generate VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Generate and bind VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Define the vertex format (positions and colors)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // Normal
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture)); // Texture coordinates
        glEnableVertexAttribArray(2);

        // Generate and bind EBO
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Destructor
    ~Mesh() 
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    // Draw function
	void Draw(unsigned int shaderProgram, Mat4 projection, Mat4 view, Mat4 model) 
    {
        // Get uniform locations for shaders
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int modelLoc = glGetUniformLocation(shaderProgram, "model");

        glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, light.position.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambient"), 1, light.ambient.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuse"), 1, light.diffuse.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "light.specular"), 1, light.specular.value_ptr());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glUniform1i(glGetUniformLocation(shaderProgram, "material.texture1"), 0);

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.value_ptr());
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.value_ptr());
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.value_ptr());

        float* mat_shine = &material.shininess;

        glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, material.ambient.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, material.diffuse.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, material.specular.value_ptr());
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), *mat_shine);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  // Use stored indices
        glBindVertexArray(0);
    }
};

#endif
