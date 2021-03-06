#include "Sphere.h"

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <iostream>


void FSphere::GetData(std::vector<float>& Data,std::vector<unsigned int>& Indices, unsigned int inSegments)
{
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;

	const float PI = 3.14159265359f;
	for (unsigned int x = 0; x <= inSegments; ++x)
	{
		for (unsigned int y = 0; y <= inSegments; ++y)
		{
			float xSegment = (float)x / (float)inSegments;
			float ySegment = (float)y / (float)inSegments;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			Positions.push_back(glm::vec3(xPos, yPos, zPos));
			UV.push_back(glm::vec2(xSegment, ySegment));
			Normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	VerticesNum = Positions.size();

	bool bOddRow = false;
	for (unsigned int y = 0; y < inSegments; ++y)
	{
		if (!bOddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= inSegments; ++x)
			{
				Indices.push_back(y * (inSegments + 1) + x);
				Indices.push_back((y + 1) * (inSegments + 1) + x);
			}
		}
		else
		{
			for (int x = inSegments; x >= 0; --x)
			{
				Indices.push_back((y + 1) * (inSegments + 1) + x);
				Indices.push_back(y * (inSegments + 1) + x);
			}
		}
		bOddRow = !bOddRow;
	}

	for (unsigned int i = 0; i < Positions.size(); ++i)
	{
		Data.push_back(Positions[i].x);
		Data.push_back(Positions[i].y);
		Data.push_back(Positions[i].z);

		Data.push_back(Normals[i].x);
		Data.push_back(Normals[i].y);
		Data.push_back(Normals[i].z);

		Data.push_back(UV[i].x);
		Data.push_back(UV[i].y);
	}
}

void FSphere::Init(unsigned int inSegments)
{
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	glGenVertexArrays(1, &SphereVAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	std::vector<unsigned int> Indices;
	std::vector<float> Data;
	GetData(Data, Indices, inSegments);

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
}

void FSphere::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[0].GetID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[1].GetID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Textures[2].GetID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Textures[3].GetID());

	glBindVertexArray(SphereVAO);

	glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
}

size_t FSphere::GetSize()
{
	return VerticesNum;
}
