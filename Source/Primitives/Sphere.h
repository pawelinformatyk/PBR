#pragma once

#include <array>
#include <vector>

#include "Texture/Texture.h"


class FSphere
{
public:

	// Send data to GPU.
	// Uses data from function GetData.
	// Data layout 
	//	vec3 - pos
	//	vec3 - normal
	//	vec2 - uv
	void Init(unsigned int inSegments);
	void Draw();
	size_t GetSize();

private:

	void GetData(std::vector<float>& Data, std::vector<unsigned int>& Indices, unsigned int inSegments);

	unsigned int SphereVAO = 0;
	unsigned int IndexCount = 0;
	size_t VerticesNum = 0;

	// Albedo
	// Normal
	// Metallic
	// Roughness
	std::array<FTexture, 4> Textures;
};