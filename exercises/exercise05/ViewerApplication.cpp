#include "ViewerApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <format>

ViewerApplication::ViewerApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(0, 30, 30)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
{
}

void ViewerApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeModel();
    InitializeCamera();
    InitializeLights();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void ViewerApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void ViewerApplication::Render()
{
    Application::Render();


    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    m_model.Draw();
    RenderGUI();

}

void ViewerApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void ViewerApplication::InitializeModel()
{
    // Load and build shader
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "C:/Users/Rasmu/Documents/2023-Graphics-Programming-2/exercises/exercise05/blinn-phong.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "C:/Users/Rasmu/Documents/2023-Graphics-Programming-2/exercises/exercise05/blinn-phong.frag");
    std::shared_ptr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
    shaderProgram->Build(vertexShader, fragmentShader);

    // Filter out uniforms that are not material properties
    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");
    filteredUniforms.insert("AmbientColor");
    filteredUniforms.insert("LightColor");
    filteredUniforms.insert("LightPosition");
    filteredUniforms.insert("CameraPosition");

    // Create reference material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgram, filteredUniforms);
    material->SetUniformValue("Color", glm::vec4(1.0f));
    material->SetUniformValue("AmbientReflection", 1.0f);
    material->SetUniformValue("DiffuseReflection", 1.0f);
    material->SetUniformValue("SpecularReflection", 1.0f);


    // Setup function
    ShaderProgram::Location worldMatrixLocation = shaderProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = shaderProgram->GetUniformLocation("ViewProjMatrix");
    ShaderProgram::Location ambientColorLocation = shaderProgram->GetUniformLocation("AmbientColor");
    ShaderProgram::Location lightColorLocation = shaderProgram->GetUniformLocation("LightColor");
    ShaderProgram::Location lightPositionLocation = shaderProgram->GetUniformLocation("LightPosition");
    ShaderProgram::Location cameraPositionLocation = shaderProgram->GetUniformLocation("CameraPosition");
    ShaderProgram::Location specularExponentLocation = shaderProgram->GetUniformLocation("SpecularExponent");


    material->SetShaderSetupFunction([=](ShaderProgram& shaderProgram)
        {
            shaderProgram.SetUniform(worldMatrixLocation, glm::scale(glm::vec3(0.1f)));
            shaderProgram.SetUniform(viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());
            shaderProgram.SetUniform(ambientColorLocation, m_ambientColor);
            shaderProgram.SetUniform(lightColorLocation, m_lightColor * m_lightIntensity);
            shaderProgram.SetUniform(lightPositionLocation, m_lightPosition);
            shaderProgram.SetUniform(cameraPositionLocation, m_cameraPosition);
            shaderProgram.SetUniform(specularExponentLocation, m_specularExponent);
            // (todo) 05.X: Set camera and light uniforms


        });

    // Configure loader
    ModelLoader loader(material);
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

    // Load model
    m_model = loader.Load("models/mill/Mill.obj");

    // (todo) 05.1: Load and set textures
    Texture2DLoader loader2d(TextureObject::FormatRGB, TextureObject::InternalFormatRGB8);
    loader2d.SetFlipVertical(true);

    loader.SetCreateMaterials(true);

    for (int i = 1; i <= m_model.GetMaterialCount(); i++)
    {
        Material& m = m_model.GetMaterial(i - 1);
        m.SetUniformValue(m.GetUniformLocation("Color"), glm::vec4(1.0f / i, 1.0f / i, 1.0f / i, 1.0f));

        if(i == 1)
            m.SetUniformValue(m.GetUniformLocation("ColorTexture0"),
                std::make_shared<Texture2DObject>(loader2d.Load("models/mill/1.jpg")));
        else if (i == 2)
            m.SetUniformValue(m.GetUniformLocation("ColorTexture0"),
                std::make_shared<Texture2DObject>(loader2d.Load("models/mill/2.jpg")));
        else
            m.SetUniformValue(m.GetUniformLocation("ColorTexture0"),
                std::make_shared<Texture2DObject>(loader2d.Load("models/mill/3.jpg")));

        m_model.SetMaterial(i - 1, std::make_shared<Material>(m));
    }


}

void ViewerApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void ViewerApplication::InitializeLights()
{
    // (todo) 05.X: Initialize light variables

}

void ViewerApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // (todo) 05.4: Add debug controls for light properties
    ImGui::DragFloat("Light intensity", &m_lightIntensity);
    ImGui::SliderFloat3("Light position", (float*)& m_lightPosition, -5000, 5000);
    ImGui::ColorEdit3("Light color", (float*)&m_lightColor);
    ImGui::DragFloat("Specular exponent", (float*)&m_specularExponent);


    m_imGui.EndFrame();
}

void ViewerApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
   {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0,1,0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

   // Update view matrix
   m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}
