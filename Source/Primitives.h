#pragma once

#include <array>
#include <vector>

#include "Texture.h"


class FPrimitiveObject 
{
public:
	// Send data to GPU.
	// Uses data from function GetData.
	// Data layout 
	//	vec3 - pos
	//	vec3 - normal
	//	vec2 - uv
	void Init();
	void Draw();

	virtual void GetData(std::vector<float>& Data, std::vector<unsigned int>& Indices) = 0;

private:
	unsigned int SphereVAO = 0;
	unsigned int IndexCount = 0;

	// Albedo
	// Normal
	// Metallic
	// Roughness
	// AO
	std::array<FTexture,5> Textures;
};


class FSphere : public FPrimitiveObject
{
public:
	FSphere(unsigned int inSegmentsX, unsigned int inSegmentsY);

	virtual void GetData(std::vector<float>& Data, std::vector<unsigned int>& Indices) override;

private:
	unsigned int SphereSegmentsX = 32;
	unsigned int SphereSegmentsY = 32;
};