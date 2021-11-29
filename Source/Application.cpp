#include "Application.h"

#include <iostream>
#include <string>
#include "stb_image.h"

int Size = 32;

Application* Application::Instance = 0;

Application::Application()
	:Camera(glm::vec3(0.0f, 0.0f, -20.f), glm::vec2(ScreenWidth / 2, ScreenHeight / 2)),
	Sphere(Size, Size),
	ShaderBase("Source/pbr_vs.glsl", "Source/pbr_fs.glsl"),
	ShaderTexture("Source/pbr_tex_vs.glsl", "Source/pbr_tex_fs.glsl"),
	ShaderPhongTexture("Source/_tex_vs.glsl", "Source/_tex_fs.glsl")
{
	Instance = this;
}

void Application::Run(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(ScreenWidth, ScreenHeight);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("PBR");

	glewInit();

	glutDisplayFunc(_Draw);
	glutIdleFunc(_Idle);
	glutMouseFunc(_MouseClick);
	glutMotionFunc(_MouseMove);
	glutKeyboardFunc(_KeyboardClick);

	glEnable(GL_DEPTH_TEST);

	Init();

	glutMainLoop();
}

void Application::Init()
{
	// Setup lights.
	glm::vec3 lightPositions[4] =
	{
		glm::vec3(-10.0f,  10.0f, -10.0f),
		glm::vec3(10.0f,  10.0f, -10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(10.0f, -10.0f, -10.0f),
	};

	Sphere.Init();

	ShaderBase.Init();
	ShaderBase.Use();
	ShaderBase.SetMat4("Projection", glm::perspective(glm::radians(60.0f), (float)ScreenWidth / ScreenHeight, 0.1f, 100.0f));
	
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		ShaderBase.SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
	}

	ShaderTexture.Init();
	ShaderTexture.Use();
	ShaderTexture.SetMat4("Projection", glm::perspective(glm::radians(60.0f), (float)ScreenWidth / ScreenHeight, 0.1f, 100.0f));

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		ShaderTexture.SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
	}

	ShaderPhongTexture.Init();
	ShaderPhongTexture.Use();
	ShaderPhongTexture.SetMat4("Projection", glm::perspective(glm::radians(60.0f), (float)ScreenWidth / ScreenHeight, 0.1f, 100.0f));

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		ShaderPhongTexture.SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
	}

	Shader = &ShaderTexture;
}

void Application::Draw()
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader->Use();

	Shader->SetMat4("View", Camera.GetView());
	Shader->SetVec3("CameraPos", Camera.GetPosition());


	const int RowsNr = 7;
	const int ColumnsNr = 7;
	const float Spacing = 2.5;

	glm::mat4 Model = glm::mat4(1.0f);

	for (int Row = 0; Row < RowsNr; ++Row)
	{
		Shader->SetFloat("Metallic", (float)Row / (float)RowsNr);
		for (int Col = 0; Col < ColumnsNr; ++Col)
		{
			// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			Shader->SetFloat("Roughness", glm::clamp((float)Col / (float)ColumnsNr, 0.05f, 1.0f));

			Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(
				(Col - (ColumnsNr / 2)) * Spacing,
				(Row - (RowsNr / 2)) * Spacing,
				0.0f
			));
			Shader->SetMat4("Model", Model);
			Sphere.Draw();
		}
	}


	glFlush();
	glutSwapBuffers();
}

void Application::Idle()
{
	glutPostRedisplay();
}

void Application::MouseClick(int button, int state, float x, float y)
{
	Camera.ProcessMouseClick(button, state, x, y);
}

void Application::MouseMove(float x, float y)
{
	Camera.ProcessMouseMove(x, y);
}

void Application::KeyboardClick(GLubyte key, int x, int y)
{
	Camera.ProcessKeyboardClick(key, x, y);

	switch (key)
	{
	case '1':
		Shader = &ShaderBase;
		break;
	case '2':
		Shader = &ShaderTexture;
		break;
	case '3':
		Shader = &ShaderPhongTexture;
		break;
	}
}


// STATIC FUNCTION CALLBACKS 

void Application::_Draw()
{
	Instance->Draw();
}

void Application::_Idle()
{
	Instance->Idle();
}

void Application::_MouseClick(int button, int state, int x, int y)
{
	Instance->MouseClick(button, state, (float)x, (float)y);
}

void Application::_MouseMove(int x, int y)
{
	Instance->MouseMove((float)x, (float)y);
}

void Application::_KeyboardClick(GLubyte key, int x, int y)
{
	Instance->KeyboardClick(key, x, y);
}