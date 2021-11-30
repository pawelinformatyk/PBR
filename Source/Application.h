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

	void Draw(GLFWwindow* Window);

	void DrawScene(GLFWwindow* Window);
	void DrawGUI();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);

private:

	void ProcessCameraInput(GLFWwindow* Window);

	void Init();

	FShaderBase ShaderPBRBase;
	FShaderTexture ShaderPBRTexture;
	FShaderTexture ShaderPhongTexture;
	FShader* Shader = nullptr;

	FCamera Camera;

	int ScreenWidth = 1600;
	int ScreenHeight = 920;

	FSphere Sphere = FSphere(32);

private:

	float IntervalBetweenLights = 20.f;
	int LightsColumns = 2;
	int LightsRows = 2;

	float IntervalBetweenModels = 5.f;
	int ModelsColumns = 5;
	int ModelsRows = 5;

	int SphereSegments = 32;

public:

	static void _FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	static void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static Application* Instance;
};
