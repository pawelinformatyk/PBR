#include "Application.h"

#include <iostream>
#include <vector>


Application* Application::Instance = 0;

Application::Application()
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
	Camera = new FCamera( glm::vec3(0.0f, 0.0f, -3.5f), glm::vec2(ScreenWidth / 2, ScreenHeight / 2));
	if (!Camera)
	{
		exit(0);
	}

	Shader = new FShader("Source/pbr_vs.glsl", "Source/pbr_fs.glsl");
	if (!Shader)
	{
		exit(0);
	}

	Shader->Use();

	const glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float)ScreenWidth / ScreenHeight, 0.1f, 100.0f);
	Shader->SetMat4("Projection", Projection);
	Shader->SetMat4("View", Camera->GetView());
}

void Application::Draw()
{
	glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Shader settings. 
	Shader->Use();

	Shader->SetMat4("View", Camera->GetView());

	const glm::mat4 Model = glm::mat4(1.0f);
	Shader->SetMat4("Model", Model);

	BuildDrawSphere();


	glFlush();
	glutSwapBuffers();
}

void Application::Idle()
{
	glutPostRedisplay();
}

void Application::MouseClick(int button, int state, float x, float y)
{
	Camera->ProcessKeyboardClick(button, state, x, y);
}

void Application::MouseMove(float x, float y)
{
	Camera->ProcessMouseMove(x, y);
}

void Application::KeyboardClick(GLubyte key, int x, int y)
{
	Camera->ProcessKeyboardClick(key, x, y);
}

void Application::BuildDrawSphere()
{
	unsigned int sphereVAO = 0;
	unsigned int indexCount = 0;

	glGenVertexArrays(1, &sphereVAO);

	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const float PI = 3.14159265359f;
	for (unsigned int x = 0; x <= SphereSegmentsX; ++x)
	{
		for (unsigned int y = 0; y <= SphereSegmentsY; ++y)
		{
			float xSegment = (float)x / (float)SphereSegmentsX;
			float ySegment = (float)y / (float)SphereSegmentsY;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < SphereSegmentsY; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= SphereSegmentsX; ++x)
			{
				indices.push_back(y * (SphereSegmentsX + 1) + x);
				indices.push_back((y + 1) * (SphereSegmentsX + 1) + x);
			}
		}
		else
		{
			for (int x = SphereSegmentsX; x >= 0; --x)
			{
				indices.push_back((y + 1) * (SphereSegmentsX + 1) + x);
				indices.push_back(y * (SphereSegmentsX + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	indexCount = indices.size();

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}

	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	unsigned int stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
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
