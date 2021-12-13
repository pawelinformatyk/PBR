#include "Application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Primitives/Primitives.h"
#include "Shaders/Shader.h"

#include <iostream>
#include <string>

#define MAX_LIGHTS 992

Application* Application::Instance = nullptr;

Application::Application()
	:Camera(glm::vec3(0.0f, -1.0f, -8.f), glm::vec2(ScreenWidth / 2, ScreenHeight / 2)),
	ShaderGouraud("Gouraud","Source/Shaders/gouraud_vs.glsl", "Source/Shaders/gouraud_fs.glsl"),
	ShaderBlinnPhong("BlinnPhong","Source/Shaders/_vs.glsl", "Source/Shaders/blinn_phong_fs.glsl"),
	ShaderPBR("PBR", "Source/Shaders/_vs.glsl", "Source/Shaders/pbr_fs.glsl"),
	ShaderTextureGouraud("Gouraud with texture", "Source/Shaders/gouraud_tex_vs.glsl", "Source/Shaders/gouraud_fs.glsl"),
	ShaderTextureBlinnPhong("BlinnPhong with texture", "Source/Shaders/_vs.glsl", "Source/Shaders/blinn_phong_tex_fs.glsl"),
	ShaderTexturePBR("PBR with texture", "Source/Shaders/_vs.glsl", "Source/Shaders/pbr_tex_fs.glsl")
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

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowMonitor(Window, nullptr, 96, 50, ScreenWidth, ScreenHeight, mode->refreshRate);

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

	ShaderGouraud.Init();
	ShaderGouraud.SetMat4("Projection", Projection);

	ShaderBlinnPhong.Init();
	ShaderBlinnPhong.SetMat4("Projection", Projection);

	ShaderPBR.Init();
	ShaderPBR.SetMat4("Projection", Projection);

	ShaderTextureGouraud.Init();
	ShaderTextureGouraud.SetMat4("Projection", Projection);

	ShaderTextureBlinnPhong.Init();
	ShaderTextureBlinnPhong.SetMat4("Projection", Projection);

	ShaderTexturePBR.Init();
	ShaderTexturePBR.SetMat4("Projection", Projection);

	ShaderOne = &ShaderTexturePBR;
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

	{
		ImGui::Begin("Settings");

		ImGui::SliderFloat("Interval Between Lights", &IntervalBetweenLights, 0.0f, 100.f);
		ImGui::SliderFloat3("Lights offset", (float*)&LightsOffset, -100.f, 100.f);
		ImGui::SliderInt("Lights Columns", &LightsColumns, 0, 31);
		ImGui::SliderInt("Lights Rows", &LightsRows, 0, 32);

		ImGui::NewLine();

		ImGui::ColorEdit3("Albedo", (float*)&Albedo);
		ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.f);
		ImGui::SliderFloat("Roughness", &Roughness, 0.05f, 0.95f);

		ImGui::NewLine();

		ImGui::SliderInt("Sphere Segments", &SphereSegments, 4, 1024);
		if (ImGui::Button("Set Sphere Segments"))
		{
			Sphere = FSphere(SphereSegments);
			Sphere.Init();
		}

		ImGui::End();
	}

	{
		ImGui::Begin("Info :");

		ImGui::Text("Shaders showed :");
		ImGui::Text(
			Scene
			? ShaderOne->GetName().c_str()
			: ("From left : 1." + ShaderGouraud.GetName() + " 2. " + ShaderBlinnPhong.GetName() + " 3. " + ShaderPBR.GetName() + " 4. " +
				ShaderTextureGouraud.GetName() + " 5. " + ShaderTextureBlinnPhong.GetName() + " 6. " + ShaderTexturePBR.GetName()).c_str());

		ImGui::NewLine();

		ImGui::Text("FPS : %.2f)", ImGui::GetIO().Framerate);
		ImGui::Text("Lights number : %d", LightsColumns * LightsRows);
		ImGui::Text("Vertices : %d", Sphere.GetSize() * (Scene ? 1 : 6));

		ImGui::End();
	}

	{
		ImGui::Begin("Input");

		ImGui::Text("Keyboard :");
		ImGui::Text("	Change scene = \"`\"");
		ImGui::Text("	Change shader to Gouraud = \"1\"");
		ImGui::Text("	Change shader to BlinnPhong = \"2\"");
		ImGui::Text("	Change shader to PBR = \"3\"");
		ImGui::Text("	Change shader to Gouraud with texture = \"4\"");
		ImGui::Text("	Change shader to BlinnPhong with texture = \"5\"");
		ImGui::Text("	Change shader to PBR with texture = \"6\"");
		ImGui::Text("	Move forward = \"w\"");
		ImGui::Text("	Move back = \"s\"");
		ImGui::Text("	Move left = \"a\"");
		ImGui::Text("	Move right = \"d\"");

		ImGui::NewLine();

		ImGui::Text("Mouse :");
		ImGui::Text("	Holding right click allows camera movement.");
		ImGui::Text("	Scroll to change movement speed.");

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::DrawScene(GLFWwindow* Window)
{
	switch (Scene)
	{
	case 0:
	{
		DrawSphere(ShaderGouraud, 6.25f);
		SetLights(ShaderGouraud);

		DrawSphere(ShaderBlinnPhong, 3.75f);
		SetLights(ShaderBlinnPhong);

		DrawSphere(ShaderPBR, 1.25f);
		SetLights(ShaderPBR);

		DrawSphere(ShaderTextureGouraud, -1.25f);
		SetLights(ShaderTextureGouraud);

		DrawSphere(ShaderTextureBlinnPhong, -3.75f);
		SetLights(ShaderTextureBlinnPhong);

		DrawSphere(ShaderTexturePBR, -6.25f);
		SetLights(ShaderTexturePBR);

		break;
	}
	case 1:
	{
		if (ShaderOne)
		{
			DrawSphere(*ShaderOne, 0.f);
			SetLights(*ShaderOne);
		}
		break;
	}
	}
}

void Application::DrawSphere(FShader& Shader, float Offset)
{
	// Draw Name as text.
	Shader.Use();

	Shader.SetMat4("View", Camera.GetView());
	Shader.SetVec3("CameraPos", Camera.GetPosition());

	Shader.SetVec3("Albedo", Albedo);
	Shader.SetFloat("Metallic", Metallic);
	Shader.SetFloat("Roughness", Roughness);

	Shader.SetInt("AlbedoMap", 0);
	Shader.SetInt("NormalMap", 1);
	Shader.SetInt("MetallicMap", 2);
	Shader.SetInt("RoughnessMap", 3);

	glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(Offset, 0.f, 0.f));

	Shader.SetMat4("Model", Model);
	Sphere.Draw();
}

void Application::SetLights(FShader& Shader)
{
	Shader.SetInt("LightsNum", LightsRows * LightsColumns);

	std::vector<glm::vec3> LightPositions;

	unsigned int Index = 0;
	for (int j = 0; j < LightsRows; ++j)
	{
		for (int i = 0; i < LightsColumns; ++i)
		{
			LightPositions.emplace_back(
				LightsOffset + 
				glm::vec3(
					(LightsColumns - 1) * IntervalBetweenLights / 2.f - IntervalBetweenLights * i,
					(LightsRows - 1) * IntervalBetweenLights / 2.f - IntervalBetweenLights * j,
					0.f));

			Shader.SetVec3("LightPositions[" + std::to_string(Index) + "]", LightPositions[Index]);
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
	if (glfwGetKey(Window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
	{
		Scene = Scene == 1 ? 0 : 1;
	}
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		ShaderOne = &ShaderGouraud;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		ShaderOne = &ShaderBlinnPhong;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		ShaderOne = &ShaderPBR;
	}
	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
	{
		ShaderOne = &ShaderTextureGouraud;
	}	
	if (glfwGetKey(Window, GLFW_KEY_5) == GLFW_PRESS)
	{
		ShaderOne = &ShaderTextureBlinnPhong;
	}
	if (glfwGetKey(Window, GLFW_KEY_6) == GLFW_PRESS)
	{
		ShaderOne = &ShaderTexturePBR;
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
