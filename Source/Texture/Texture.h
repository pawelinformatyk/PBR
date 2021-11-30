#pragma once

#include <glad/glad.h>

class FTexture
{
public:
	GLuint GetID() const { return ID; };
	void LoadTextureFromFile(const char* file_name);

private:
	GLuint ID;
};