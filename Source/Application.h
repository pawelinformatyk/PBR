#pragma once

struct GLFWwindow;

#include "Primitives/Primitives.h"
#include "Shaders/Shader.h"
#include "Camera/Camera.h"


class Application
{
public:

	Application();

	void Run(int argc, char** argv);

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:

	void Init();

	void ProcessCameraInput(GLFWwindow* Window);

	void Draw(GLFWwindow* Window);
	void DrawScene(GLFWwindow* Window);
	void DrawGUI();
	void DrawSphere(FShader& Shader,float Offset);
	void SetLights(FShader& Shader);

	FShader ShaderGouraud;
	FShader ShaderBlinnPhong;
	FShader ShaderPBR;
	FShader ShaderTextureGouraud;
	FShader ShaderTextureBlinnPhong;
	FShader ShaderTexturePBR;
	FShader* ShaderOne;

	FCamera Camera;

	int ScreenWidth = int(1920. * 0.9);
	int ScreenHeight = int(1080. * 0.9);

	FSphere Sphere = FSphere(32);
private:

	float IntervalBetweenLights = 20.f;
	int LightsColumns = 2;
	int LightsRows = 2;

	int SphereSegments = 32;

	float Metallic = 0.f;
	float Roughness = 0.05f;
	glm::vec3 Albedo = glm::vec3(0.5f, 0.f, 0.f);
	glm::vec3 LightsOffset = glm::vec3(0.f, 0.f, -10.f);

	int Scene = 0;

public:

	static void _FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	static void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void _ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static Application* Instance;
};
