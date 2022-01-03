#pragma once

struct GLFWwindow;

#include "Primitives/Sphere.h"
#include "Shaders/Shader.h"
#include "Camera/Camera.h"
#include "vector"

enum class EScene
{
	EDemo,
	EStudy,
};

class Application
{
public:

	Application();

	void Run();

private:

	void Init();
	void Begin();
	void End();

	void Draw();
	void DrawScene();
	void DrawGUI();

	void DrawSphere(FShader& Shader,float Offset);
	void SetLights(FShader& Shader);

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:

	std::vector<FShader> Shaders;
	FShader* ShaderOne = nullptr;

	FCamera Camera;

	int ScreenWidth = int(1920. * 0.9);
	int ScreenHeight = int(1080. * 0.9);

	FSphere Sphere;

	float IntervalBetweenLights = 20.f;
	glm::vec3 LightsOffset = glm::vec3(0.f, 0.f, -10.f);
	int LightsColumns = 2;
	int LightsRows = 2;

	float Metallic = 0.f;
	float Roughness = 0.05f;
	glm::vec3 Albedo = glm::vec3(0.5f, 0.f, 0.f);

	int SphereSegments = 32;

	EScene Scene = EScene::EDemo;

	GLFWwindow* Window = nullptr;

private:

	static void _FramebufferSizeCallback(GLFWwindow* inWindow, int Width, int Height);
	static void _KeyCallback(GLFWwindow* inWindow, int key, int scancode, int action, int mods);
	static void _ScrollCallback(GLFWwindow* inWindow, double xoffset, double yoffset);

	static Application* Instance;
};
