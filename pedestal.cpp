#include "pedestal.h"

const int SLICES = 50;
const int STACKS = 3;
const int NUM_VA_VERTICES = (SLICES + 1) * (STACKS + 1);
const int NUM_VA_INDICES = (SLICES + 1) * (STACKS) * 6;
const int NUM_DEBUG_VERTICES = NUM_VA_VERTICES * 2;
const int NUM_DEBUG_INDICES = NUM_DEBUG_VERTICES;

const GLfloat RADIUS = 6;
const GLfloat HEIGHT = 1;

const GLfloat RADIAN_CONVERSION = GLfloat(3.14159) / 180;
const GLfloat NORMAL_LENGTH_SCALE = GLfloat(0.2);

Pedestal::Pedestal()
{
	pedestalDisplayList = -1;
	debugDisplayList = -1;

	debug = false;
}

Pedestal::~Pedestal()
{
}

void Pedestal::ToggleDebug()
{
	debug = !debug;
}

void Pedestal::CreatePedestalDisplayList()
{
	pedestalDisplayList = glGenLists(1);
	glNewList(pedestalDisplayList, GL_COMPILE);

	glMatrixMode(GL_MODELVIEW);

	// these are only computed/generated once
	ComputeVaVertices();
	GenerateVaIndices();
	ComputeVaNormals();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glColor3f(1, 1, 1);

	glVertexPointer(3, GL_FLOAT, 0, &vaVertices[0]);
	glNormalPointer(GL_FLOAT, 0, &vaNormals[0]);
	glDrawElements(GL_TRIANGLES, NUM_VA_INDICES, GL_UNSIGNED_INT, &vaIndices[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glEndList();
}

void Pedestal::CreateDebugDisplayList()
{
	debugDisplayList = glGenLists(1);
	glNewList(debugDisplayList, GL_COMPILE);
	
	glMatrixMode(GL_MODELVIEW);

	// these are only computed/generated once
	ComputeDebugVertices();
	GenerateDebugIndices();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	glVertexPointer(3, GL_FLOAT, 0, &debugVertices[0]);
	glDrawElements(GL_LINES, NUM_DEBUG_VERTICES, GL_UNSIGNED_INT, &debugIndices[0]);

	glEnable(GL_LIGHTING);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glEndList();
}

void Pedestal::Display()
{
	if (pedestalDisplayList == -1)
	{
		CreatePedestalDisplayList();
	}

	if (debugDisplayList == -1)
	{
		CreateDebugDisplayList();
	}

	glCallList(pedestalDisplayList);

	if (debug)
	{
		glCallList(debugDisplayList);
	}
}

void Pedestal::ComputeVaVertices()
{
	// this splits the entire pedestal into SLICES + 1 slices. The calculation uses SLICES + 1 because
	// one slice equals two pieces
	float angleIncrement = 360 / (SLICES + 1) * RADIAN_CONVERSION;

	// there will always be three slices
	for (int i = 0; i < STACKS + 1; i++)
	{
		for (int j = 0; j < SLICES + 1; j++)
		{
			// the bottom-center "circle" is compressed down to a single point
			if (i == 0)
			{
				vaVertices.push_back(0);
				vaVertices.push_back(0);
				vaVertices.push_back(0);
			}
			// the top-center "circle" is also compressed down to a single point
			else if (i == 3)
			{
				vaVertices.push_back(0);
				vaVertices.push_back(HEIGHT);
				vaVertices.push_back(0);
			}
			// the other circles really are circles
			else
			{
				vaVertices.push_back(cos(angleIncrement * j) * RADIUS);
				vaVertices.push_back((i - 1) * HEIGHT);
				vaVertices.push_back(sin(angleIncrement * j) * RADIUS);
			}
		}
	}
}

void Pedestal::GenerateVaIndices()
{
	// this functions behaves in exactly the same way as Balloon::GenerateVaIndices

	for (int i = 0; i < STACKS; i++)
	{
		for (int j = 0; j < SLICES + 1; j++)
		{
			// first triangle
			vaIndices.push_back(i * (SLICES + 1) + j);

			if (j == SLICES)
			{
				vaIndices.push_back(i * (SLICES + 1));
			}
			else
			{
				vaIndices.push_back(i * (SLICES + 1) + j + 1);
			}

			vaIndices.push_back((i + 1) * (SLICES + 1) + j);

			// second triangle
			vaIndices.push_back(i * (SLICES + 1) + j);
			vaIndices.push_back((i + 1) * (SLICES + 1) + j);

			if (j == 0)
			{
				vaIndices.push_back((i + 2) * (SLICES + 1) - 1);
			}
			else
			{
				vaIndices.push_back((i + 1) * (SLICES + 1) + j - 1);
			}
		}
	}
}

void Pedestal::ComputeVaNormals()
{
	for (int i = 0; i < STACKS + 1; i++)
	{
		for (int j = 0; j < SLICES + 1; j++)
		{
			// for the bottom of the pedestal, the normals point straight down
			if (i == 0)
			{
				vaNormals.push_back(0);
				vaNormals.push_back(-1);
				vaNormals.push_back(0);
			}
			// for the top of the pedestal, the normals point straight up
			else if (i == 3)
			{
				vaNormals.push_back(0);
				vaNormals.push_back(1);
				vaNormals.push_back(0);
			}
			// for the cylinder part, the normals point straight out from the center
			else
			{
				int vaIndex = (i * (SLICES + 1) + j) * 3;

				GLfloat x = vaVertices.at(vaIndex);
				GLfloat y = vaVertices.at(vaIndex + 1);
				GLfloat z = vaVertices.at(vaIndex + 2);
				GLfloat magnitude = glm::length(glm::vec3(x, y, z));

				vaNormals.push_back(vaVertices.at(vaIndex) / magnitude);
				vaNormals.push_back(0);
				vaNormals.push_back(vaVertices.at(vaIndex + 2) / magnitude);
			}
		}
	}
}

void Pedestal::ComputeDebugVertices()
{
	// this functions behaves in exactly the same way as Balloon::GenerateDebugVertices

	for (int i = 0; i < NUM_VA_VERTICES; i++)
	{
		// coordinates of the vertex
		float vX = vaVertices.at(i * 3);
		float vY = vaVertices.at(i * 3 + 1);
		float vZ = vaVertices.at(i * 3 + 2);

		// coordinates of the vertex plus its normal
		float nX = vX + vaNormals.at(i * 3) * NORMAL_LENGTH_SCALE;
		float nY = vY + vaNormals.at(i * 3 + 1) * NORMAL_LENGTH_SCALE;
		float nZ = vZ + vaNormals.at(i * 3 + 2) * NORMAL_LENGTH_SCALE;

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
	// this functions behaves in exactly the same way as Balloon::GenerateDebugIndices

	for (int i = 0; i < NUM_DEBUG_INDICES; i++)
	{
		debugIndices.push_back(i);
	}
}