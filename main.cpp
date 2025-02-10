#define STB_IMAGE_IMPLEMENTATION

#include <mesh.h>
#include <camera.h>
#include <mat4.h>
#include <vec3.h>
#include <vec2.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stb_image.h>

float resolutionX = 1920;
float resolutionY = 1080;

Camera camera;
bool moveCamera = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Light light;

// Resize window
void static ResizeWindow(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

// Player input
void static PlayerInput(GLFWwindow* window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);

        moveCamera = true;
    }
    else
    {
        moveCamera = false;
    }

    if (!moveCamera)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            light.position.z += 100 * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            light.position.z -= 100 * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            light.position.x -= 100 * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            light.position.x += 100 * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            light.position.y -= 100 * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            light.position.y += 100 * deltaTime;

    }
}

void static MouseInput(GLFWwindow* window, double posX, double posY) 
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        static float lastX = resolutionX / 2.0f;
        static float lastY = resolutionY / 2.0f;
        static bool firstMouse = true;

        if (firstMouse)
        {
            lastX = posX;
            lastY = posY;
            firstMouse = false;
            return;
        }

        float xoffset = posX - lastX;
        float yoffset = lastY - posY; // Reversed since y-coordinates go bottom to top

        lastX = posX;
        lastY = posY;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// Shader
std::string static ReadShaderSource(const std::string& filename) 
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int static CompileShader(unsigned int shaderType, const std::string& shaderSource) 
{
    unsigned int shader = glCreateShader(shaderType);
    const char* src = shaderSource.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

	// Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) 
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int static CreateShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) 
{
    // Read shader sources
    std::string vertexShaderSource = ReadShaderSource(vertexShaderPath);
    std::string fragmentShaderSource = ReadShaderSource(fragmentShaderPath);

    // Compile shaders
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) 
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders (no longer needed once linked)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}

// Texture
unsigned int static LoadTexture(const char* path) 
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // Load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	// Check if image was loaded successfully
    if (data) 
    {
		// Bind the texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Horizontal wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  // Vertical wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the texture to OpenGL
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } 
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

		// Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data); // Free the image memory
    }
	else // Failed to load texture
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Load OBJ file
bool static loadOBJ(const char* filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    // Open the OBJ file
    std::ifstream file(filename);

	// Check if the file was opened successfully
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open .obj file: " << filename << std::endl;
        return false;
    }

    // Temporary storage for vertex data
    std::vector<Vec3> tempVertexPos;    // Vertex positions
    std::vector<Vec3> tempVertexNorm;     // Vertex normals
    std::vector<Vec2> tempTexCoords;   // Texture coordinates

    // Read file line by line
    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream s(line);
        std::string prefix;
        s >> prefix;

        if (prefix == "v") // Vertex position
        {
            Vec3 position;
            s >> position.x >> position.y >> position.z;
            tempVertexPos.push_back(position);
        }
        else if (prefix == "vt") // Texture coordinate
        {
            Vec2 texCoord;
            s >> texCoord.x >> texCoord.y;
            tempTexCoords.push_back(texCoord);
        }
		else if (prefix == "vn") // Vertex normal
        {
            Vec3 normal;
            s >> normal.x >> normal.y >> normal.z;
            tempVertexNorm.push_back(normal);
        }
		else if (prefix == "f") // Face (triangle or quad)
        {
            std::vector<std::string> faceData;
            std::string vertexData;

            // Collect all vertex data for this face
            while (s >> vertexData) {
                faceData.push_back(vertexData);
            }

            // If the face is a quad, split it into two triangles
			if (faceData.size() == 4) // Quad face
            {
                // Triangle 1: vertices 1, 2, 3
                for (int i : {0, 1, 2})
                {
                    std::istringstream viss(faceData[i]);
                    std::string posIndexStr, texIndexStr, normIndexStr;

                    // Parse vertex, texture, and normal indices
                    std::getline(viss, posIndexStr, '/');
                    std::getline(viss, texIndexStr, '/');
                    std::getline(viss, normIndexStr, '/');

                    // Convert to 0-based indices
                    unsigned int posIndex = (posIndexStr.empty()) ? 0 : std::stoi(posIndexStr) - 1;
                    unsigned int texIndex = (texIndexStr.empty()) ? 0 : std::stoi(texIndexStr) - 1;
                    unsigned int normIndex = (normIndexStr.empty()) ? 0 : std::stoi(normIndexStr) - 1;

                    // Create vertex
                    Vertex vertex;
                    vertex.position = tempVertexPos[posIndex];
                    vertex.texture = (texIndex < tempTexCoords.size()) ? tempTexCoords[texIndex] : Vec2(0.0f, 0.0f);
                    vertex.normal = (normIndex < tempVertexNorm.size()) ? tempVertexNorm[normIndex] : Vec3(0.0f, 0.0f, 0.0f);

                    // Add vertex and index
                    vertices.push_back(vertex);
                    indices.push_back(vertices.size() - 1);
                }

                // Triangle 2: vertices 1, 3, 4
                for (int i : {0, 2, 3}) 
                {
                    std::istringstream viss(faceData[i]);
                    std::string posIndexStr, texIndexStr, normIndexStr;

                    // Parse vertex, texture, and normal indices
                    std::getline(viss, posIndexStr, '/');
                    std::getline(viss, texIndexStr, '/');
                    std::getline(viss, normIndexStr, '/');

                    // Convert to 0-based indices
                    unsigned int posIndex = (posIndexStr.empty()) ? 0 : std::stoi(posIndexStr) - 1;
                    unsigned int texIndex = (texIndexStr.empty()) ? 0 : std::stoi(texIndexStr) - 1;
                    unsigned int normIndex = (normIndexStr.empty()) ? 0 : std::stoi(normIndexStr) - 1;

                    // Create vertex
                    Vertex vertex;
                    vertex.position = tempVertexPos[posIndex];
                    vertex.texture = (texIndex < tempTexCoords.size()) ? tempTexCoords[texIndex] : Vec2(0.0f, 0.0f);
                    vertex.normal = (normIndex < tempVertexNorm.size()) ? tempVertexNorm[normIndex] : Vec3(0.0f, 0.0f, 0.0f);

                    // Add vertex and index
                    vertices.push_back(vertex);
                    indices.push_back(vertices.size() - 1);
                }
            }
            else if (faceData.size() == 3)// Triangle face
            {
                for (int i : {0, 1, 2})
                {
                    std::istringstream viss(faceData[i]);
                    std::string posIndexStr, texIndexStr, normIndexStr;

                    // Parse vertex, texture, and normal indices
                    std::getline(viss, posIndexStr, '/');
                    std::getline(viss, texIndexStr, '/');
                    std::getline(viss, normIndexStr, '/');

                    // Convert to 0-based indices
                    unsigned int posIndex = (posIndexStr.empty()) ? 0 : std::stoi(posIndexStr) - 1;
                    unsigned int texIndex = (texIndexStr.empty()) ? 0 : std::stoi(texIndexStr) - 1;
                    unsigned int normIndex = (normIndexStr.empty()) ? 0 : std::stoi(normIndexStr) - 1;

                    // Create vertex
                    Vertex vertex;
                    vertex.position = tempVertexPos[posIndex];
                    vertex.texture = (texIndex < tempTexCoords.size()) ? tempTexCoords[texIndex] : Vec2(0.0f, 0.0f);
                    vertex.normal = (normIndex < tempVertexNorm.size()) ? tempVertexNorm[normIndex] : Vec3(0.0f, 0.0f, 0.0f);

                    // Add vertex and index
                    vertices.push_back(vertex);
                    indices.push_back(vertices.size() - 1);
                }
            }
			else // Unsupported face type
            {
                std::cerr << "Unsupported face type with " << faceData.size() << " vertices." << std::endl;
            }
        }
    }

    file.close();

    return true;
}

int main() 
{
    // Initialize GLFW
    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    
    // Create a windowed mode window
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "MG3D", nullptr, nullptr);
    if (!window) 
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, ResizeWindow);
    glfwSetCursorPosCallback(window, MouseInput);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Create a shader program
    unsigned int shaderProgram = CreateShaderProgram("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
    
    // Loadd textures
    unsigned int texture1 = LoadTexture("textures/Scratches-Textures.jpg");
    unsigned int texture2 = LoadTexture("textures/Triangles-Textures.png");
    unsigned int texture3 = LoadTexture("textures/Color-Textures.jpg");
    unsigned int texture4 = LoadTexture("textures/Vibrant-Swirl-Textures-1.jpg");
    
    // Define vertex data
    std::vector<Vertex> vertices1 = {
        {Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f)},  // Bottom-left
        {Vec3(0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 0.0f)},  // Bottom-right
        {Vec3(0.0f,  0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.5f, 1.0f)}   // Top
    };
    
    // Define index data
    std::vector<unsigned int> indices1
    {
        0, 1, 2 // indices for the first triangle
    };
    
    // Define material properties
    Material material1 = {
        texture1,
        Vec3(0.1f, 0.1f, 0.1f),  // Ambient
        Vec3(0.8f, 0.0f, 0.0f),  // Diffuse
        Vec3(1.0f, 1.0f, 1.0f),  // Specular
        32.0f                         // Shininess
    };

    Material material2 = {
        texture1,
        Vec3(0.5f, 0.5f, 0.5f),  // Ambient
        Vec3(0.4f, 0.4f, 0.4f),  // Diffuse
        Vec3(1.0f, 1.0f, 1.0f),  // Specular
        32.0f                         // Shininess
    };

    light.position = Vec3(-4.0f, 3.0f, 0.0f);
    light.ambient = Vec3(0.5f, 0.5f, 0.5f);
    light.diffuse = Vec3(0.7f, 0.7f, 0.7f);
    light.specular = Vec3(1.0f, 1.0f, 1.0f);
    
    // Create a mesh
    Mesh triangle(vertices1, indices1, material1, light, texture1);
    
    // Load OBJ file - box
    std::vector<Vertex> vertices_box;
    std::vector<unsigned int> indices_box;
    if (!loadOBJ("assets/box.obj", vertices_box, indices_box)) {
        std::cerr << "Failed to load OBJ file!" << std::endl;
        return -1;
    }
    
    Mesh box(vertices_box, indices_box, material2, light, texture4);
    
    // Load OBJ file - sphere
    std::vector<Vertex> vertices_sphere;
    std::vector<unsigned int> indices_sphere;
    if (!loadOBJ("assets/sphere.obj", vertices_sphere, indices_sphere)) {
        std::cerr << "Failed to load OBJ file!" << std::endl;
        return -1;
    }
    
    Mesh sphere(vertices_sphere, indices_sphere, material1, light, texture2);
    
    // Load OBJ file - cylinder
    std::vector<Vertex> vertices_cylinder;
    std::vector<unsigned int> indices_cylinder;
    if (!loadOBJ("assets/cylinder.obj", vertices_cylinder, indices_cylinder)) {
        std::cerr << "Failed to load OBJ file!" << std::endl;
        return -1;
    }
    
    Mesh cylinder(vertices_cylinder, indices_cylinder, material2, light, texture3);

    
    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
    
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    
        PlayerInput(window);
    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, camera.position.value_ptr());

        Mat4 view = camera.GetViewMatrix();
        Mat4 projection = Mat4().Perspective(degreeToRadians(45.0f), (float)1920 / 1080, 0.1f, 100.0f);
        Mat4 model = Mat4();
    
        model = model * Mat4().Scale(0.5f, 0.5f, 0.5f);
        model = model * Mat4().Translate(0, 1, -3);
        triangle.Draw(shaderProgram, projection, view, model);
    
        model = Mat4();
        model = model * Mat4().Scale(0.5f, 0.5f, 0.5f);
        model = model * Mat4().RotateZ(45);
        model = model * Mat4().Translate(-1, 0, -3);
        box.Draw(shaderProgram, projection, view, model);
    
        model = Mat4();
        model = model * Mat4().Scale(0.5f, 0.5f, 0.5f);
        model = model * Mat4().Translate(0, 0, -3);
        sphere.Draw(shaderProgram, projection, view, model);
    
        model = Mat4();
        model = model * Mat4().Scale(0.5f, 0.5f, 0.5f);
        model = model * Mat4().RotateX(45);
        model = model * Mat4().Translate(1, 0, -3);
        cylinder.Draw(shaderProgram, projection, view, model);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

