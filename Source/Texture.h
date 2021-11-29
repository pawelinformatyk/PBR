#pragma once

#include "glew.h"

class FTexture
{
public:
	GLuint GetID()const { return ID; };
	void LoadTextureFromFile(const char* file_name);

private:
	GLuint ID;
};