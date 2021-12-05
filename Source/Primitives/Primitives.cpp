#include "Primitives.h"

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <iostream>


FSphere::FSphere(unsigned int inSegmentsX, unsigned int inSegmentsY)
{
	SphereSegmentsX = inSegmentsX;
	SphereSegmentsY = inSegmentsY;	
}

FSphere::FSphere(unsigned int inSegments)
{
	SphereSegmentsX = inSegments;
	SphereSegmentsY = inSegments;
}

void FSphere::GetData(std::vector<float>& Data,std::vector<unsigned int>& Indices)
{
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;

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

			Positions.push_back(glm::vec3(xPos, yPos, zPos));
			UV.push_back(glm::vec2(xSegment, ySegment));
			Normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool bOddRow = false;
	for (unsigned int y = 0; y < SphereSegmentsY; ++y)
	{
		if (!bOddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= SphereSegmentsX; ++x)
			{
				Indices.push_back(y * (SphereSegmentsX + 1) + x);
				Indices.push_back((y + 1) * (SphereSegmentsX + 1) + x);
			}
		}
		else
		{
			for (int x = SphereSegmentsX; x >= 0; --x)
			{
				Indices.push_back((y + 1) * (SphereSegmentsX + 1) + x);
				Indices.push_back(y * (SphereSegmentsX + 1) + x);
			}
		}
		bOddRow = !bOddRow;
	}
	
	VerticesNum =  Positions.size();

	for (unsigned int i = 0; i < Positions.size(); ++i)
	{
		Data.push_back(Positions[i].x);
		Data.push_back(Positions[i].y);
		Data.push_back(Positions[i].z);
		if (Normals.size() > 0)
		{
			Data.push_back(Normals[i].x);
			Data.push_back(Normals[i].y);
			Data.push_back(Normals[i].z);
		}
		if (UV.size() > 0)
		{
			Data.push_back(UV[i].x);
			Data.push_back(UV[i].y);
		}
	}
}

void FPrimitiveObject::Init()
{
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	glGenVertexArrays(1, &SphereVAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	std::vector<unsigned int> Indices;
	std::vector<float> Data;
	GetData(Data, Indices);

	IndexCount = static_cast<unsigned int>(Indices.size());

	glBindVertexArray(SphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Data.size() * sizeof(float), &Data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

	unsigned int Stride = (3 + 3 + 2) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Stride, (void*)(6 * sizeof(float)));

	Textures[0].LoadTextureFromFile("Textures/rustediron2_albedo.png");
	Textures[1].LoadTextureFromFile("Textures/rustediron2_normal.png");
	Textures[2].LoadTextureFromFile("Textures/rustediron2_metallic.png");
	Textures[3].LoadTextureFromFile("Textures/rustediron2_roughness.png");
	Textures[4].LoadTextureFromFile("Textures/rustediron2_ao.png");
}

void FPrimitiveObject::Draw()
{
	glBindVertexArray(SphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[0].GetID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[1].GetID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Textures[2].GetID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Textures[3].GetID());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Textures[4].GetID());
}

size_t FPrimitiveObject::GetSize()
{
	return VerticesNum;
}
