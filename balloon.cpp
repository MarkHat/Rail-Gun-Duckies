#include "balloon.h"

Balloon::Balloon()
{
	vaDisplayListHandle = -1;
	debugDisplayListHandle = -1;

	debug = false;

	slices = 50;
	stacks = 50;
	radius = 2;

	numVaVertices = (slices + 1) * (stacks + 1);
	numVaIndices = (slices + 1) * (stacks) * 6;
	numDebugVertices = numVaVertices * 2;
	numDebugIndices = numDebugVertices;
}

Balloon::~Balloon()
{
}

float Balloon::GetRadius()
{
	return radius;
}

glm::vec3 Balloon::GetPosition()
{
	return position;
}

void Balloon::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Balloon::ToggleDebug()
{
	debug = !debug;
}

void Balloon::Display()
{
	//glMatrixMode(GL_MODELVIEW);

	if (vaDisplayListHandle == -1)
	{
		vaDisplayListHandle = glGenLists(1);
		glNewList(vaDisplayListHandle, GL_COMPILE);

		ComputeVaVertices();
		GenerateVaIndices();
		ComputeVaNormals();

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		// reference: http://www.opengl.org/sdk/docs/man/xhtml/glVertexPointer.xml
		// reference: http://www.opengl.org/sdk/docs/man/xhtml/glNormalPointer.xml
		glVertexPointer(3, GL_FLOAT, 0, &vaVertices[0]);
		glNormalPointer(GL_FLOAT, 0, &vaNormals[0]);

		// reference: http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml
		glDrawElements(GL_TRIANGLES, numVaIndices, GL_UNSIGNED_INT, &vaIndices[0]);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glEndList();
	}

	if (debugDisplayListHandle == -1)
	{
		debugDisplayListHandle = glGenLists(1);
		glNewList(debugDisplayListHandle, GL_COMPILE);

		ComputeDebugVertices();
		GenerateDebugIndices();

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		
		glColor3f(1, 1, 1);
		glDisable(GL_LIGHTING);
		glVertexPointer(3, GL_FLOAT, 0, &debugVertices[0]);
		glDrawElements(GL_LINES, numDebugVertices, GL_UNSIGNED_INT, &debugIndices[0]);
		glEnable(GL_LIGHTING);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glEndList();
	}

	glCallList(vaDisplayListHandle);

	if (debug)
	{
		glCallList(debugDisplayListHandle);
	}
}

void Balloon::ComputeVaVertices()
{
	float pi = 3.14159;
	float radianConversion = pi / 180;
	float sliceAngleIncrement = 360 / slices * radianConversion;
	float sinAngleIncrement = pi / (stacks + 1);
	float heightIncrement = radius * 2 / stacks;

	for (int i = 0; i < stacks + 1; i++)
	{
		float currentY;
		float currentRadius;

		// top half
		if (i <= (stacks + 1) / 2)
		{
			currentY = radius - i * heightIncrement;
			currentRadius = glm::sqrt(radius * radius - currentY * currentY);
		}
		// bottom half
		else
		{
			currentY = (radius - i * heightIncrement) * 1.5;
			currentRadius = glm::sin(pi / 2 + sinAngleIncrement * ((stacks + 1) / 2 - i)) * radius;
		}

		for (int j = 0; j < slices + 1; j++)
		{
			vaVertices.push_back(glm::cos(sliceAngleIncrement * j) * currentRadius);
			vaVertices.push_back(currentY);
			vaVertices.push_back(glm::sin(sliceAngleIncrement * j) * currentRadius);
		}
	}
}

void Balloon::GenerateVaIndices()
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

void Balloon::ComputeVaNormals()
{
	for (int i = 0; i < stacks + 1; i++)
	{
		for (int j = 0; j < slices + 1; j++)
		{
			int vaIndex = (i * (slices + 1) + j) * 3;

			// top half
			if (i <= (stacks + 1) / 2)
			{
				float magnitude = sqrt(vaVertices.at(vaIndex) * vaVertices.at(vaIndex) +
									   vaVertices.at(vaIndex + 1) * vaVertices.at(vaIndex + 1) +
									   vaVertices.at(vaIndex + 2) * vaVertices.at(vaIndex + 2));

				vaNormals.push_back(vaVertices.at(vaIndex) / magnitude);
				vaNormals.push_back(vaVertices.at(vaIndex + 1) / magnitude);
				vaNormals.push_back(vaVertices.at(vaIndex + 2) / magnitude);
			}
			// bottom half
			else
			{
				float pi = 3.14159;
				float sinAngleIncrement = pi / (stacks + 1);

				vaNormals.push_back(vaVertices.at(vaIndex));
				vaNormals.push_back(-glm::cos(pi / 2 + sinAngleIncrement * ((stacks + 1) / 2 - i)) / 1.5);
				vaNormals.push_back(vaVertices.at(vaIndex + 2));

				int normalIndex = vaNormals.size() - 3;
				float magnitude = sqrt(vaNormals.at(normalIndex) * vaNormals.at(normalIndex) +
									   vaNormals.at(normalIndex + 1) * vaNormals.at(normalIndex + 1) +
									   vaNormals.at(normalIndex + 2) * vaNormals.at(normalIndex + 2));

				vaNormals.at(normalIndex) /= magnitude;
				vaNormals.at(normalIndex + 1) /= magnitude;
				vaNormals.at(normalIndex + 2) /= magnitude;
			}
		}
	}
}

void Balloon::ComputeDebugVertices()
{
	for (int i = 0; i < numVaVertices; i++)
	{
		// coordinates of the vertex
		float vX = vaVertices.at(i * 3);
		float vY = vaVertices.at(i * 3 + 1);
		float vZ = vaVertices.at(i * 3 + 2);

		// coordinates of the vertex plus its normal
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

void Balloon::GenerateDebugIndices()
{
	for (int i = 0; i < numDebugIndices; i++)
	{
		debugIndices.push_back(i);
	}
}