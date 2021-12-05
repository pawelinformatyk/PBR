#pragma once


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

struct GLFWwindow;


class FCamera
{
public:
	FCamera();
	FCamera(glm::vec3 CameraPos, glm::vec2 MousePosition);

	void ProcessInput(GLFWwindow* Window);

	// Calculate view from new looking direction (mouse position).
	void CalculateView(double x, double y);

	void AddSpeed(float Val);

	glm::mat4& GetView();
	glm::vec3& GetPosition();

private:

	glm::mat4 View = glm::mat4(0.f);

	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 DirectionFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 DirectionUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float Speed = .25f;

	bool bRightButtonPressed = false;
	double MouseX = 0.f;
	double MouseY = 0.f;

	double Yaw = 90.0f;
	double Pitch = 0.f;

	const double RotationSensitivity = 0.1f;
};