#pragma once

#include "glew.h"
#include "freeglut.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 


class FCamera
{
public:
	FCamera();
	FCamera(glm::vec3 CameraPos, glm::vec2 MousePosition);

	void ProcessMouseMove(float x, float y);
	void ProcessMouseClick(int button, int state, float x, float y);
	void ProcessKeyboardClick(GLubyte key, int x, int y);

	// Calculate view from new looking direction (mouse position).
	void CalculateView(float x, float y);

	glm::mat4& GetView();
	glm::vec3& GetPosition();

private:

	glm::mat4 View = glm::mat4(0.f);

	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 DirectionFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 DirectionUp = glm::vec3(0.0f, 1.0f, 0.0f);

	const float Speed = 1.f;

	bool bRightButtonPressed = false;
	float MouseX = 0.f;
	float MouseY = 0.f;

	float Yaw = 90.0f;
	float Pitch = 0.f;

	const float RotationSensitivity = 0.1f;
};