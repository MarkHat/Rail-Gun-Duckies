#include "pedestal.h"

Pedestal::Pedestal()
{
	debug = false;

	slices = 50;
	stacks = 3;
	radius = 6;
	height = 1;

	numVaVertices = (slices + 1) * (stacks + 1);
	numVaIndices = (slices + 1) * (stacks) * 6;
	numDebugVertices = numVaVertices * 2;
	numDebugIndices = numDebugVertices;
}

Pedestal::~Pedestal()
{
}

void Pedestal::ToggleDebug()
{
	debug = !debug;
}

void Pedestal::Display()
{
	glMatrixMode(GL_MODELVIEW);

	if (vaVertices.size() == 0)
	{
		ComputeVaVertices();
		GenerateVaIndices();
		ComputeVaNormals();

		ComputeDebugVertices();
		GenerateDebugIndices();
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glColor3f(1, 1, 1);

	glVertexPointer(3, GL_FLOAT, 0, &vaVertices[0]);
	glNormalPointer(GL_FLOAT, 0, &vaNormals[0]);
	glDrawElements(GL_TRIANGLES, numVaIndices, GL_UNSIGNED_INT, &vaIndices[0]);
	
	if (debug)
	{
		glColor3f(1, 1, 1);
		glDisable(GL_LIGHTING);
		glVertexPointer(3, GL_FLOAT, 0, &debugVertices[0]);
		glDrawElements(GL_LINES, numDebugVertices, GL_UNSIGNED_INT, &debugIndices[0]);
		glEnable(GL_LIGHTING);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void Pedestal::ComputeVaVertices()
{
	float pi = 3.14159;
	float radianConversion = pi / 180;
	float angleIncrement = 360 / slices * radianConversion;

	for (int i = 0; i < stacks + 1; i++)
	{
		for (int j = 0; j < slices + 1; j++)
		{
			if (i == 0)
			{
				vaVertices.push_back(0);
				vaVertices.push_back(0);
				vaVertices.push_back(0);
			}
			else if (i == 3)
			{
				vaVertices.push_back(0);
				vaVertices.push_back(height);
				vaVertices.push_back(0);
			}
			else
			{
				vaVertices.push_back(cos(angleIncrement * j) * radius);
				vaVertices.push_back((i - 1) * height);
				vaVertices.push_back(sin(angleIncrement * j) * radius);
			}
		}
	}
}

void Pedestal::GenerateVaIndices()
{
	for (int i = 0; i < stacks; i++)
	{
		for (int j = 0; j < slices + 1; j++)
		{
			// first triangle
			vaIndices.push_back(i * (slices + 1) + j);

			if (j == slices)
			{
				vaIndices.push_back(i * (slices + 1));
			}
			else
			{
				vaIndices.push_back(i * (slices + 1) + j + 1);
			}

			vaIndices.push_back((i + 1) * (slices + 1) + j);

			// second triangle
			vaIndices.push_back(i * (slices + 1) + j);
			vaIndices.push_back((i + 1) * (slices + 1) + j);

			if (j == 0)
			{
				vaIndices.push_back((i + 2) * (slices + 1) - 1);
			}
			else
			{
				vaIndices.push_back((i + 1) * (slices + 1) + j - 1);
			}
		}
	}
}

void Pedestal::ComputeVaNormals()
{
	for (int i = 0; i < stacks + 1; i++)
	{
		for (int j = 0; j < slices + 1; j++)
		{
			// bottom of the pedestal
			if (i == 0)
			{
				vaNormals.push_back(0);
				vaNormals.push_back(-1);
				vaNormals.push_back(0);
			}
			// top of the pedestal
			else if (i == 3)
			{
				vaNormals.push_back(0);
				vaNormals.push_back(1);
				vaNormals.push_back(0);
			}
			// cylinder part
			else
			{
				int vaIndex = (i * (slices + 1) + j) * 3;
				float magnitude = sqrt(vaVertices.at(vaIndex) * vaVertices.at(vaIndex) +
									   vaVertices.at(vaIndex + 2) * vaVertices.at(vaIndex + 2));

				vaNormals.push_back(vaVertices.at(vaIndex) / magnitude);
				vaNormals.push_back(0);
				vaNormals.push_back(vaVertices.at(vaIndex + 2) / magnitude);
			}
		}
	}
}

void Pedestal::ComputeDebugVertices()
{
	for (int i = 0; i < numVaVertices; i++)
	{
		float vX = vaVertices.at(i * 3);
		float vY = vaVertices.at(i * 3 + 1);
		float vZ = vaVertices.at(i * 3 + 2);

		float nX = vX + vaNormals.at(i * 3) * 0.2;
		float nY = vY + vaNormals.at(i * 3 + 1) * 0.2;
		float nZ = vZ + vaNormals.at(i * 3 + 2) * 0.2;

		debugVertices.push_back(vX);
		debugVertices.push_back(vY);
		debugVertices.push_back(vZ);

		debugVertices.push_back(nX);
		debugVertices.push_back(nY);
		debugVertices.push_back(nZ);
	}
}

void Pedestal::GenerateDebugIndices()
{
	for (int i = 0; i < numDebugIndices; i++)
	{
		debugIndices.push_back(i);
	}
}