#include "ducky.h"

Ducky::Ducky()
{
	debug = false;

	duckyDisplayListHandle = -1;
	boundingBoxDisplayListHandle = -1;

	oldPosition = glm::vec3(0, 0, 0);
	newPosition = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	bounds = glm::vec3(1, 1, 1);
}

Ducky::~Ducky()
{
}

void Ducky::ToggleDebug()
{
	debug = !debug;
}

void Ducky::CreateDuckyDisplayList()
{
	duckyDisplayListHandle = glGenLists(1);
	glNewList(duckyDisplayListHandle, GL_COMPILE);

	glPushMatrix();
	glColor3f(1, 1, 0);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluSphere.xml
	// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluNewQuadric.xml
	// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluDeleteQuadric.xml
	GLUquadric * quadric = gluNewQuadric();

	// reference: http://blog.repertoiremag.com/wp-content/uploads/2010/09/rubber-ducky1.jpg
	// body
	glPushMatrix();
	glScalef(2, 2, 2);
	gluSphere(quadric, .25, 25, 25);
	glPopMatrix();

	// left wing
	glPushMatrix();
	glTranslatef(0, .1, -.5);
	glRotatef(10, 1, 0, 0);
	glScalef(0.6, 0.3, 0.2);
	gluSphere(quadric, 1, 25, 25);
	glPopMatrix();

	// right wing
	glPushMatrix();
	glTranslatef(0, 0.1, 0.5);
	glRotatef(-10, 1, 0, 0);
	glScalef(0.6, 0.3, 0.2);
	gluSphere(quadric, 1, 25, 25);
	glPopMatrix();

	// head
	glPushMatrix();
	glScalef(1.2, 1.1, 1.2);
	glTranslatef(.3, .6, 0);
	gluSphere(quadric, .25, 25, 25);
	glPopMatrix();

	// beak
	glPushMatrix();
	glTranslatef(.6, .7, 0);
	glColor3f(1, .64, 0);
	glScalef(.5, .7, 1);
	glRotatef(90, 0, 1, 0);
	gluCylinder(quadric, .1, 0, .5, 20, 20);
	glPopMatrix();

	// eyes
	glPushMatrix();
	glScalef(1.2, 1, 1);
	glTranslatef(.5, .8, .15);
	glColor3f(0, 0, 0);
	gluSphere(quadric, .05, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glScalef(1.2, 1, 1);
	glTranslatef(.5, .8, -.15);
	glColor3f(0,0,0);
	gluSphere(quadric, .05, 20, 20);
	glPopMatrix();

	gluDeleteQuadric(quadric);
	glPopMatrix();

	glEndList();
}

void Ducky::CreateBoundingBoxDisplayList()
{
	boundingBoxDisplayListHandle = glGenLists(1);
	glNewList(boundingBoxDisplayListHandle, GL_COMPILE);
	glPushMatrix();

	// the ducky's bounding box is just a rectangular prism surrounding the duck, so I'm
	// simply computing each vertex manually
	glm::vec3 halfBounds = glm::vec3(bounds.x / 2, bounds.y / 2, bounds.z / 2);

	glm::vec3 boxVertices[8] = {
		// top half of the box
		glm::vec3(-halfBounds.x, halfBounds.y, -halfBounds.z),
		glm::vec3(-halfBounds.x, halfBounds.y, halfBounds.z),
		glm::vec3(halfBounds.x, halfBounds.y, halfBounds.z),
		glm::vec3(halfBounds.x, halfBounds.y, -halfBounds.z),

		// bottom half of the box
		glm::vec3(-halfBounds.x, -halfBounds.y, -halfBounds.z),
		glm::vec3(-halfBounds.x, -halfBounds.y, halfBounds.z),
		glm::vec3(halfBounds.x, -halfBounds.y, halfBounds.z),
		glm::vec3(halfBounds.x, -halfBounds.y, -halfBounds.z)
	};

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

	// front
	glVertex3f(boxVertices[3].x, boxVertices[3].y, boxVertices[3].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);
	glVertex3f(boxVertices[0].x, boxVertices[0].y, boxVertices[0].z);

	// back
	glVertex3f(boxVertices[1].x, boxVertices[1].y, boxVertices[1].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[2].x, boxVertices[2].y, boxVertices[2].z);

	// bottom
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[5].x, boxVertices[5].y, boxVertices[5].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[6].x, boxVertices[6].y, boxVertices[6].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[7].x, boxVertices[7].y, boxVertices[7].z);
	glVertex3f(boxVertices[4].x, boxVertices[4].y, boxVertices[4].z);

	glEnd();
	glPopMatrix();
	glEndList();
}

void Ducky::Display()
{
	if (duckyDisplayListHandle == -1)
	{
		CreateDuckyDisplayList();
	}

	if (boundingBoxDisplayListHandle == -1)
	{
		CreateBoundingBoxDisplayList();
	}

	glCallList(duckyDisplayListHandle);

	if (debug)
	{
		glCallList(boundingBoxDisplayListHandle);
	}
}

glm::vec3 Ducky::GetOldPosition()
{
	return oldPosition;
}

glm::vec3 Ducky::GetNewPosition()
{
	return newPosition;
}

glm::vec3 Ducky::GetVelocity()
{
	return velocity;
}

glm::vec3 Ducky::GetBounds()
{
	return bounds;
}

void Ducky::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	newPosition.x = x;
	newPosition.y = y;
	newPosition.z = z;

	oldPosition.x = newPosition.x;
	oldPosition.y = newPosition.y;
	oldPosition.z = newPosition.z;
}

void Ducky::SetVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void Ducky::Update(double elapsedTime, GLfloat gravity)
{
	oldPosition.x = newPosition.x;
	oldPosition.y = newPosition.y;
	oldPosition.y = newPosition.y;

	newPosition.x += velocity.x * elapsedTime;
	newPosition.y += velocity.y * elapsedTime;
	newPosition.z += velocity.z * elapsedTime;

	// apply gravity
	velocity.y += gravity;
}