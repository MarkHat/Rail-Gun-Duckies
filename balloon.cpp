#include "balloon.h"

const int SLICES = 50;
const int STACKS = 50;
const int NUM_VA_VERTICES = (SLICES + 1) * (STACKS + 1);
const int NUM_VA_INDICES = (SLICES + 1) * (STACKS) * 6;
const int NUM_DEBUG_VERTICES = NUM_VA_VERTICES * 2;
const int NUM_DEBUG_INDICES = NUM_DEBUG_VERTICES;

const GLfloat RADIUS = 2;
const GLfloat PI = GLfloat(3.14159);
const GLfloat RADIAN_CONVERSION = PI / GLfloat(180.0);

const GLfloat BOTTOM_HALF_HEIGHT_SCALE = GLfloat(1.5);
const GLfloat NORMAL_LENGTH_SCALE = GLfloat(0.2);

const GLfloat TORUS_INNER_RADIUS = GLfloat(0.1);
const GLfloat TORUS_OUTER_RADIUS = GLfloat(0.1);
const int TORUS_SIDES = 20;
const int TORUS_RINGS = 20;

const glm::vec3 BOUNDS = glm::vec3(RADIUS * 2, RADIUS * (1 + BOTTOM_HALF_HEIGHT_SCALE), RADIUS * 2);

Balloon::Balloon()
{
	vaDisplayListHandle = -1;
	normalDisplayListHandle = -1;
	boundingBoxDisplayListHandle = -1;

	debug = false;
	position = glm::vec3(0, 0, 0);
}

Balloon::~Balloon()
{
}

float Balloon::GetRadius()
{
	return RADIUS;
}

glm::vec3 Balloon::GetPosition()
{
	return position;
}

glm::vec3 Balloon::GetBounds()
{
	return BOUNDS;
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

void Balloon::CreateBalloonDisplayList()
{
	vaDisplayListHandle = glGenLists(1);
	glNewList(vaDisplayListHandle, GL_COMPILE);

	// these will only be computed/generated once
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
	glDrawElements(GL_TRIANGLES, NUM_VA_INDICES, GL_UNSIGNED_INT, &vaIndices[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// draw the torus
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, RADIUS * BOTTOM_HALF_HEIGHT_SCALE);
	glutSolidTorus(TORUS_INNER_RADIUS, TORUS_OUTER_RADIUS, TORUS_SIDES, TORUS_RINGS);
	glPopMatrix();

	glEndList();
}

void Balloon::CreateNormalDisplayList()
{
	normalDisplayListHandle = glGenLists(1);
	glNewList(normalDisplayListHandle, GL_COMPILE);

	// these will only be computed/generated once
	ComputeDebugVertices();
	GenerateDebugIndices();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	// draw the normals
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	glVertexPointer(3, GL_FLOAT, 0, &debugVertices[0]);
	glDrawElements(GL_LINES, NUM_DEBUG_VERTICES, GL_UNSIGNED_INT, &debugIndices[0]);
	glEnable(GL_LIGHTING);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glEndList();
}

void Balloon::CreateBoundingBoxDisplayList()
{
	boundingBoxDisplayListHandle = glGenLists(1);
	glNewList(boundingBoxDisplayListHandle, GL_COMPILE);
	glPushMatrix();

	// each balloon's bounding box is just a rectangular prism surrounding the balloon, so I'm
	// simply computing each vertex manually
	glm::vec3 halfBounds = glm::vec3(BOUNDS.x / 2, BOUNDS.y / 2, BOUNDS.z / 2);
	float quarterRadius = RADIUS / 4;

	glm::vec3 boxVertices[8] = {
		// top half of the box
		glm::vec3(-halfBounds.x, halfBounds.y - quarterRadius, -halfBounds.z),
		glm::vec3(-halfBounds.x, halfBounds.y - quarterRadius, halfBounds.z),
		glm::vec3(halfBounds.x, halfBounds.y - quarterRadius, halfBounds.z),
		glm::vec3(halfBounds.x, halfBounds.y - quarterRadius, -halfBounds.z),

		// bottom half of the box
		glm::vec3(-halfBounds.x, -halfBounds.y - quarterRadius, -halfBounds.z),
		glm::vec3(-halfBounds.x, -halfBounds.y - quarterRadius, halfBounds.z),
		glm::vec3(halfBounds.x, -halfBounds.y - quarterRadius, halfBounds.z),
		glm::vec3(halfBounds.x, -halfBounds.y - quarterRadius, -halfBounds.z)
	};

	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);

	// top
	glVertex3f(boxVertices[0].x, boxVertices[0].y, boxVertices[0].z);
	glVertex3f(boxVertices[1].x, boxVertices[1].y, boxVertices[1].z);
	glVertex3f(boxVertices[1].x, boxVertices[1].y, boxVertices[1].z);
	glVertex3f(boxVertices[2].x, boxVertices[2].y, boxVertices[2].z);
	glVertex3f(boxVertices[2].x, boxVertices[2].y, boxVertices[2].z);
	glVertex3f(boxVertices[3].x, boxVertices[3].y, boxVertices[3].z);
	glVertex3f(boxVertices[3].x, boxVertices[3].y, boxVertices[3].z);
	glVertex3f(boxVertices[0].x, boxVertices[0].y, boxVertices[0].z);

	// bottom
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);

	// vertical lines (there are four)
	glVertex3f(boxVertices[3].x, boxVertices[3].y, boxVertices[3].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);
	glVertex3f(boxVertices[0].x, boxVertices[0].y, boxVertices[0].z);
	glVertex3f(boxVertices[1].x, boxVertices[1].y, boxVertices[1].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[2].x, boxVertices[2].y, boxVertices[2].z);

	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEndList();
}

void Balloon::Display()
{
	if (vaDisplayListHandle == -1)
	{
		CreateBalloonDisplayList();
	}

	if (normalDisplayListHandle == -1)
	{
		CreateNormalDisplayList();
	}

	if (boundingBoxDisplayListHandle == -1)
	{
		CreateBoundingBoxDisplayList();
	}

	glCallList(vaDisplayListHandle);

	if (debug)
	{
		glCallList(normalDisplayListHandle);
		glCallList(boundingBoxDisplayListHandle);
	}
}

void Balloon::ComputeVaVertices()
{
	// sliceAngleIncrement is used to split up the balloon into SLICES + 1 "slices" (like a pie). The
	// value is SLICES + 1 because one slice gives you two pieces
	float sliceAngleIncrement = 360 / SLICES * RADIAN_CONVERSION;

	// sinAngleIncrement is used to compute the radii for points on the bottom half of the balloon. The
	// points on the bottom half of the balloon are computed using a transformed sin wave, so 
	// sinAngleIncrement splits the range pi to 2 * pi into (STACKS + 1) / 2 uniform angles
	float sinAngleIncrement = PI / (STACKS + 1);

	// points on the balloon are computed top to bottom at consistent height differences. To do this,
	// heightIncrement splits RADIUS * 2 into STACKS equal heights
	float heightIncrement = RADIUS * 2 / STACKS;

	for (int i = 0; i < STACKS + 1; i++)
	{
		float currentY;
		float currentRadius;

		// the top half is a hemisphere
		if (i <= (STACKS + 1) / 2)
		{
			currentY = RADIUS - i * heightIncrement;
			currentRadius = glm::sqrt(RADIUS * RADIUS - currentY * currentY);
		}
		// the bottom half is a transformed sin wave
		else
		{
			currentY = (RADIUS - i * heightIncrement) * BOTTOM_HALF_HEIGHT_SCALE;
			currentRadius = glm::sin(PI / 2 + sinAngleIncrement * ((STACKS + 1) / 2 - i)) * RADIUS;
		}

		// add the physical vertices
		for (int j = 0; j < SLICES + 1; j++)
		{
			vaVertices.push_back(glm::cos(sliceAngleIncrement * j) * currentRadius);
			vaVertices.push_back(currentY);
			vaVertices.push_back(glm::sin(sliceAngleIncrement * j) * currentRadius);
		}
	}
}

void Balloon::GenerateVaIndices()
{
	// the indices are computed by essentially thinking of the balloon as a
	// wrapped-around lattice of points that looks like this:
	// . _ . _ .
	// | \ | \ |
	// . _ . _ .
	// | \ | \ |
	// . _ . _ .

	for (int i = 0; i < STACKS; i++)
	{
		for (int j = 0; j < SLICES + 1; j++)
		{
			// first triangle
			// . _ .
			//   \ |
			//     .
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
			// .
			// | \
			// . _ .
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

void Balloon::ComputeVaNormals()
{
	for (int i = 0; i < STACKS + 1; i++)
	{
		for (int j = 0; j < SLICES + 1; j++)
		{
			// vertices are stored as sets of three floats
			int vaIndex = (i * (SLICES + 1) + j) * 3;

			GLfloat x = vaVertices.at(vaIndex);
			GLfloat y = vaVertices.at(vaIndex + 1);
			GLfloat z = vaVertices.at(vaIndex + 2);
			GLfloat magnitude = glm::length(glm::vec3(x, y, z));

			// top half of the balloon
			if (i <= (STACKS + 1) / 2)
			{
				// for the top half, normals point straight away from the center of the balloon
				vaNormals.push_back(vaVertices.at(vaIndex) / magnitude);
				vaNormals.push_back(vaVertices.at(vaIndex + 1) / magnitude);
				vaNormals.push_back(vaVertices.at(vaIndex + 2) / magnitude);
			}
			// bottom half of the balloon
			else
			{
				float sinAngleIncrement = PI / (STACKS + 1);

				vaNormals.push_back(vaVertices.at(vaIndex));

				// for the bottom half, normals correspond to the derivative of the transformed sin wave
				vaNormals.push_back(-glm::cos(PI / 2 + sinAngleIncrement * ((STACKS + 1) / 2 - i)) / BOTTOM_HALF_HEIGHT_SCALE);

				vaNormals.push_back(vaVertices.at(vaIndex + 2));

				// normalize the normals
				int normalIndex = vaNormals.size() - 3;

				GLfloat nX = vaNormals.at(normalIndex);
				GLfloat nY = vaNormals.at(normalIndex + 1);
				GLfloat nZ = vaNormals.at(normalIndex + 2);
				GLfloat magnitude = glm::length(glm::vec3(nX, nY, nZ));

				vaNormals.at(normalIndex) /= magnitude;
				vaNormals.at(normalIndex + 1) /= magnitude;
				vaNormals.at(normalIndex + 2) /= magnitude;
			}
		}
	}
}

void Balloon::ComputeDebugVertices()
{
	// for each balloon vertex, there is one normal. To draw the normal, a line is drawn
	// from the vertex to the vertex plus its normal
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

void Balloon::GenerateDebugIndices()
{
	// normals (a.k.a. lines) are drawn between every two successive points in
	// the debug vertex array
	for (int i = 0; i < NUM_DEBUG_INDICES; i++)
	{
		debugIndices.push_back(i);
	}
}