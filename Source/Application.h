#pragma once

#include "glew.h"
#include "freeglut.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

#include "Shader.h"
#include "Camera.h"
#include "Primitives.h"


class Application
{
public:

	Application();

	void Run(int argc, char** argv);

	void Draw();
	void Idle();
	void MouseClick(int button, int state, float x, float y);
	void MouseMove(float x, float y);
	void KeyboardClick(GLubyte key, int x, int y);

	static void _Draw();
	static void _Idle();
	static void _MouseClick(int button, int state, int x, int y);
	static void _MouseMove(int x, int y);
	static void _KeyboardClick(GLubyte key, int x, int y);

private:

	void Init();

	FShaderBase ShaderBase;
	FShaderTexture ShaderTexture;
	FShaderTexture ShaderPhongTexture;
	FShader* Shader=nullptr;

	FCamera Camera;

	int ScreenWidth = 1600;
	int ScreenHeight = 920;

	FSphere Sphere;

protected:
	static Application* Instance;
};