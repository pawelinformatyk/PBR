#include "Application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Primitives/Primitives.h"
#include "Shaders/Shader.h"

#include <iostream>


Application* Application::Instance = 0;

Application::Application()
	:Camera(glm::vec3(0.0f, 0.0f, -20.f), glm::vec2(ScreenWidth / 2, ScreenHeight / 2)),
	ShaderPBR("Source/Shaders/vertex_shader.glsl", "Source/Shaders/pbr_fs.glsl"),
	ShaderTexturePBR("Source/Shaders/vertex_shader.glsl", "Source/Shaders/pbr_tex_fs.glsl"),
	ShaderBlinnPhong("Source/Shaders/vertex_shader.glsl", "Source/Shaders/blinn_phong_fs.glsl"),
	ShaderTextureBlinnPhong("Source/Shaders/vertex_shader.glsl", "Source/Shaders/blinn_phong_tex_fs.glsl")
{
	Instance = this;
}

void Application::Run(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* Window = glfwCreateWindow(ScreenWidth, ScreenHeight, "PBR", NULL, NULL);
	if (!Window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(Window);

	glfwSetFramebufferSizeCallback(Window, &Application::_FramebufferSizeCallback);
	glfwSetKeyCallback(Window, &Application::_KeyCallback);
	glfwSetScrollCallback(Window, &Application::_ScrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO(); (void)IO;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(Window))
	{
		Camera.ProcessInput(Window);
		Draw(Window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Application::Init()
{
	Sphere.Init();

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float)ScreenWidth / ScreenHeight, 0.01f, 10000.0f);

	ShaderPBR.Init();
	ShaderPBR.SetMat4("Projection", Projection);

	ShaderTexturePBR.Init();
	ShaderTexturePBR.SetMat4("Projection", Projection);

	ShaderBlinnPhong.Init();
	ShaderBlinnPhong.SetMat4("Projection", Projection);

	ShaderTextureBlinnPhong.Init();
	ShaderTextureBlinnPhong.SetMat4("Projection", Projection);

	Shader = &ShaderTexturePBR;
}

void Application::Draw(GLFWwindow* Window)
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawScene(Window);
	DrawGUI();

	glfwSwapBuffers(Window);
	glfwPollEvents();
}

void Application::DrawGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings!");

	ImGui::SliderFloat("IntervalBetweenLights", &IntervalBetweenLights, 0.0f, 100.f);
	ImGui::SliderInt("LightsColumns", &LightsColumns, 0, 16);
	ImGui::SliderInt("LightsRows", &LightsRows, 0, 16);

	ImGui::SliderFloat("IntervalBetweenModels", &IntervalBetweenModels, 0.0f, 100.f);
	ImGui::SliderInt("ModelsColumns", &ModelsColumns, 0, 32);
	ImGui::SliderInt("ModelsRows", &ModelsRows, 0, 32);

	ImGui::NewLine();

	ImGui::SliderInt("SphereSegments", &SphereSegments, 4, 1024);

	if (ImGui::Button("ButtonSetSphereSegments"))
	{
		Sphere = FSphere(SphereSegments);
		Sphere.Init();
	}

	ImGui::NewLine();

	ImGui::Text("FPS : %.2f)", ImGui::GetIO().Framerate);
	ImGui::Text("Lights number : %d", LightsColumns * LightsRows);
	ImGui::Text("Models number : %d", ModelsColumns * ModelsRows);
	ImGui::Text("Vertices : %d", Sphere.GetSize() * ModelsColumns * ModelsRows);

	// TODO :
	// ImGui::Text("Shader used : %s", );

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::DrawScene(GLFWwindow* Window)
{
	Shader->Use();

	Shader->SetMat4("View", Camera.GetView());
	Shader->SetVec3("CameraPos", Camera.GetPosition());

	glm::mat4 Model = glm::mat4(1.0f);

	// Roughness : left higher 
	// Metallic  : up higher
	for (int i = 0; i < ModelsRows; ++i)
	{
		Shader->SetFloat("Metallic", (float)i / (float)ModelsRows);
		for (int j = 0; j < ModelsColumns; ++j)
		{
			Shader->SetFloat("Roughness", glm::clamp((float)j / (float)ModelsColumns, 0.05f, 1.0f));

			Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(
				(j - (ModelsColumns / 2)) * IntervalBetweenModels,
				(i - (ModelsRows / 2)) * IntervalBetweenModels,
				0.0f
			));
			Shader->SetMat4("Model", Model);

			Sphere.Draw();
		}
	}

	// Setup lights.
	Shader->SetInt("LightsNum", LightsRows * LightsColumns);

	std::vector<glm::vec3> LightPositions;

	unsigned int Index = 0;
	for (int j = 0; j < LightsRows; ++j)
	{
		for (int i = 0; i < LightsColumns; ++i)
		{
			LightPositions.emplace_back(
				glm::vec3(
					(LightsColumns - 1) * IntervalBetweenLights / 2.f - IntervalBetweenLights * i,
					(LightsRows - 1) * IntervalBetweenLights / 2.f - IntervalBetweenLights * j,
					-10.f));

			Shader->SetVec3("LightPositions[" + std::to_string(Index) + "]", LightPositions[Index]);
			++Index;
		}
	}
}

void Application::KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		Shader = &ShaderPBR;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		Shader = &ShaderBlinnPhong;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		Shader = &ShaderTexturePBR;
	}
	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
	{
		Shader = &ShaderTextureBlinnPhong;
	}
}

void Application::FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

void Application::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera.AddSpeed((float)yoffset/100.f);
}

// STATIC FUNCTION CALLBACKS 

void Application::_FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	Instance->FramebufferSizeCallback(Window, Width, Height);
}

void Application::_KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	Instance->KeyCallback(Window, Key, ScanCode, Action, Mods);
}

void Application::_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Instance->ScrollCallback(window, xoffset, yoffset);
}
