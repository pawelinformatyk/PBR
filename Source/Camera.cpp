#include "Camera.h"


FCamera::FCamera(glm::vec3 CameraPos, glm::vec2 MousePosition)
{
	View = glm::translate(glm::mat4(1.f), CameraPos);
	Position = CameraPos;
	MouseX = MousePosition.x;
	MouseY = MousePosition.y;
}

FCamera::FCamera()
{

}

void FCamera::ProcessMouseMove(float x, float y)
{
	if (!bRightButtonPressed)
	{
		return;
	}

	float Xoffset = x - MouseX;
	float Yoffset = MouseY - y;
	MouseX = x;
	MouseY = y;

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

void FCamera::ProcessKeyboardClick(int button, int state, float x, float y)
{
	if (button != GLUT_RIGHT_BUTTON)
	{
		return;
	}

	switch (state)
	{
	case GLUT_UP:
		bRightButtonPressed = false;
		break;
	case GLUT_DOWN:
		bRightButtonPressed = true;
		MouseX = x;
		MouseY = y;
		break;
	}
}

void FCamera::ProcessKeyboardClick(GLubyte key, int x, int y)
{
	if (!bRightButtonPressed)
	{
		return;
	}

	switch (key)
	{
	case 'w':
		Position += Speed * DirectionFront;
		break;
	case 's':
		Position -= Speed * DirectionFront;
		break;
	case 'a':
		Position -= Speed * glm::normalize(glm::cross(DirectionFront, DirectionUp));
		break;
	case 'd':
		Position += Speed * glm::normalize(glm::cross(DirectionFront, DirectionUp));
		break;
	}

	View = glm::lookAt(
		Position,
		Position + DirectionFront,
		DirectionUp);
}

glm::mat4& FCamera::GetView()
{
	return View;
}

