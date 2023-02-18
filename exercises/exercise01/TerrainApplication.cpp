#include "TerrainApplication.h"

// (todo) 01.1: Include the libraries you need

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <array>
#include <ituGL/geometry/VertexAttribute.h>

// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

struct Vertex
{
    Vertex(Vector3 position, Vector2 texture_coordinates, Vector3 color) 
        : position(position), texture_coordinates(texture_coordinates), color(color) {}

    Vector2 texture_coordinates;
    Vector3 position, color;
};

// (todo) 01.8: Declare an struct with the vertex format



TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(50), m_gridY(50), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();

    Vector2 scale(1.0f / m_gridX, 1.0f / m_gridY);

    // (todo) 01.1: Create containers for the vertex position
    std::vector<Vector3> v_position;
    std::vector<Vector2> v_text_coord;
    std::vector<Vector3> v_color;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= m_gridX; i++)
    {
        for (int j = 0; j <= m_gridY; j++)
        {  
            float z_val = stb_perlin_fbm_noise3((scale.x * i - 0.5f) * 2, (scale.y * j - 0.5f) * 2, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
            v_position.push_back(Vector3(scale.x * i - 0.5 , scale.y * j - 0.5, z_val));
            v_text_coord.push_back(Vector2(i, j));


            if (z_val > 0.3f)
                v_color.push_back(Vector3(1.0f, 1.0f, 1.0f));
            else if (z_val > 0.1f)
                v_color.push_back(Vector3(0.3, 0.3f, 0.35f));
            else if (z_val > -0.05f)
                v_color.push_back(Vector3(0.1, 0.4f, 0.15f));
            else if (z_val > -0.1f)
                v_color.push_back(Vector3(0.6, 0.5f, 0.4f));
            else
                v_color.push_back(Vector3(0.1f, 0.1f, 0.3f));


            if (i < m_gridX && j < m_gridY) {
                int bottom_left = i * (m_gridY + 1) + j;
                int top_left = bottom_left + 1;
                int bottom_right = top_left + m_gridY;
                int top_right = bottom_right + 1;

                indices.push_back(bottom_left);
                indices.push_back(top_left);
                indices.push_back(bottom_right);

                indices.push_back(top_left);
                indices.push_back(bottom_right);
                indices.push_back(top_right);
            }
        } 
    }

    m_vao.Bind();
    m_vbo.Bind();
    m_ebo.Bind();

    VertexAttribute position(Data::Type::Float, 3);
    VertexAttribute texture(Data::Type::Float, 2);
    VertexAttribute color(Data::Type::Float, 3);

    size_t data_size = v_position.size() * position.GetSize() +
                       v_text_coord.size() * texture.GetSize() +
                       v_color.size() * color.GetSize();

    size_t v_position_offset = v_position.size() * position.GetSize();
    size_t v_text_coord_offset = v_position_offset + v_text_coord.size() * texture.GetSize();

    m_ebo.AllocateData(std::span(indices));
    m_vbo.AllocateData(data_size);
    
    m_vbo.UpdateData(std::span(v_position));
    m_vbo.UpdateData(std::span(v_text_coord), v_position_offset);
    m_vbo.UpdateData(std::span(v_color), v_text_coord_offset);

    m_vao.SetAttribute(0, position, 0);
    m_vao.SetAttribute(1, texture, v_position_offset);
    m_vao.SetAttribute(2, color, v_text_coord_offset);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // (todo) 01.1: Fill in vertex data


    // (todo) 01.1: Initialize VAO, and VBO


    // (todo) 01.5: Initialize EBO


    // (todo) 01.1: Unbind VAO, and VBO
    m_vbo.Unbind();
    m_vao.Unbind();


    // (todo) 01.5: Unbind EBO
    m_ebo.Unbind();

}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glEnable(GL_DEPTH_TEST);

    glUseProgram(m_shaderProgram);

    m_vao.Bind();

    // (todo) 01.1: Draw the grid
    //glDrawArrays(GL_TRIANGLES, 0, (m_gridX + 1) * (m_gridY + 1));
    glDrawElements(GL_TRIANGLES, 2 * 3 * m_gridX * m_gridY, GL_UNSIGNED_INT, 0);
}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}

void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}