#include "Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


FCamera::FCamera(glm::vec3 CameraPos, glm::vec2 MousePosition)
{
	View = glm::translate(glm::mat4(1.f), CameraPos);
	Position = CameraPos;
	MouseX = MousePosition.x;
	MouseY = MousePosition.y;

	CalculateView(0, 0);
}

FCamera::FCamera()
{

}

void FCamera::CalculateView(double NewX, double NewY)
{
	double Xoffset = NewX - MouseX;
	double Yoffset = MouseY - NewY;
	MouseX = NewX;
	MouseY = NewY;

	Xoffset *= RotationSensitivity;
	Yoffset *= RotationSensitivity;

	Yaw += Xoffset;
	Pitch += Yoffset;

	glm::vec3 Direction = glm::vec3(
		cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
		sin(glm::radians(Pitch)),
		sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)));

	DirectionFront = glm::normalize(Direction);

	View = glm::lookAt(
		Position,
		Position + DirectionFront,
		DirectionUp);
}

void FCamera::ProcessInput(GLFWwindow* Window)
{
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Position += Speed * DirectionFront;
		}
		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Position -= Speed * DirectionFront;
		}
		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Position -= Speed * glm::normalize(glm::cross(DirectionFront, DirectionUp));
		}
		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Position += Speed * glm::normalize(glm::cross(DirectionFront, DirectionUp));
		}

		if (!bRightButtonPressed)
		{
			glfwGetCursorPos(Window, &MouseX, &MouseY);
			View = glm::lookAt(
				Position,
				Position + DirectionFront,
				DirectionUp);
		}
		else
		{
			double NewX, NewY;
			glfwGetCursorPos(Window, &NewX, &NewY);
			CalculateView(NewX, NewY);
		}

		bRightButtonPressed = true;
	}
	else
	{
		bRightButtonPressed = false;
	}
}

glm::mat4& FCamera::GetView()
{
	return View;
}

glm::vec3& FCamera::GetPosition()
{
	return Position;
}

