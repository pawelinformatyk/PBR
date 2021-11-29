#include "Primitives.h"

#include "glew.h"
#include "freeglut.h"

#include "glm/glm.hpp"

#include <iostream>


FSphere::FSphere(unsigned int inSegmentsX, unsigned int inSegmentsY)
{
	SphereSegmentsX = inSegmentsX;
	SphereSegmentsY = inSegmentsY;
}

void FSphere::GetData(std::vector<float>& Data,std::vector<unsigned int>& Indices)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uv;

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
		oddRow = !oddRow;
	}
	std::cout << positions.size() << std::endl;
	std::cout << positions.size() * 49 << std::endl;

	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		Data.push_back(positions[i].x);
		Data.push_back(positions[i].y);
		Data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			Data.push_back(normals[i].x);
			Data.push_back(normals[i].y);
			Data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			Data.push_back(uv[i].x);
			Data.push_back(uv[i].y);
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

	IndexCount = Indices.size();

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

	Textures[0].LoadTextureFromFile("Source/rustediron2_albedo.png");
	Textures[1].LoadTextureFromFile("Source/rustediron2_normal.png");
	Textures[2].LoadTextureFromFile("Source/rustediron2_metallic.png");
	Textures[3].LoadTextureFromFile("Source/rustediron2_roughness.png");
	Textures[4].LoadTextureFromFile("Source/rustediron2_ao.png");
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
