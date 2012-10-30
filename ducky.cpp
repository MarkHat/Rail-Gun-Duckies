#include "ducky.h"

const glm::vec3 BOUNDS = glm::vec3(1, 1, 1);

const int SLICES = 25;
const int STACKS = 25;

const int WING_TILT = 10;
const glm::vec3 WING_OFFSET = glm::vec3(0, 0.1, 0.5);
const glm::vec3 WING_SCALE = glm::vec3(0.6, 0.3, 0.2);

const glm::vec3 HEAD_OFFSET = glm::vec3(0.3, 0.6, 0);
const glm::vec3 HEAD_SCALE = glm::vec3(0.3, 0.3, 0.3);

const glm::vec3 BEAK_COLOR = glm::vec3(1, 0.64, 0);
const glm::vec3 BEAK_OFFSET = glm::vec3(0.55, 0.65, 0);
const glm::vec3 BEAK_SCALE = glm::vec3(0.7, 0.7, 0.7);

const GLfloat EYE_RADIUS = 0.05;
const glm::vec3 EYE_OFFSET = glm::vec3(0.5, 0.76, 0.15);

Ducky::Ducky()
{
	debug = false;

	duckyDisplayListHandle = -1;
	boundingBoxDisplayListHandle = -1;

	oldPosition = glm::vec3(0, 0, 0);
	newPosition = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
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
	gluSphere(quadric, 0.5, SLICES, STACKS);
	glPopMatrix();

	// left wing
	glPushMatrix();
	glTranslatef(WING_OFFSET.x, WING_OFFSET.y, -WING_OFFSET.z);
	glRotatef(WING_TILT, 1, 0, 0);
	glScalef(WING_SCALE.x, WING_SCALE.y, WING_SCALE.z);
	gluSphere(quadric, 1, SLICES, STACKS);
	glPopMatrix();

	// right wing
	glPushMatrix();
	glTranslatef(WING_OFFSET.x, WING_OFFSET.y, WING_OFFSET.z);
	glRotatef(-WING_TILT, 1, 0, 0);
	glScalef(WING_SCALE.x, WING_SCALE.y, WING_SCALE.z);
	gluSphere(quadric, 1, SLICES, STACKS);
	glPopMatrix();

	// head
	glPushMatrix();
	glTranslatef(HEAD_OFFSET.x, HEAD_OFFSET.y, HEAD_OFFSET.z);
	glScalef(HEAD_SCALE.x, HEAD_SCALE.y, HEAD_SCALE.z);
	gluSphere(quadric, 1, SLICES, STACKS);
	glPopMatrix();

	// beak
	glPushMatrix();
	glColor3f(BEAK_COLOR.x, BEAK_COLOR.y, BEAK_COLOR.z);
	glTranslatef(BEAK_OFFSET.x, BEAK_OFFSET.y, BEAK_OFFSET.z);
	glScalef(BEAK_SCALE.x, BEAK_SCALE.y, BEAK_SCALE.z);
	glRotatef(90, 0, 1, 0);
	gluCylinder(quadric, 0.1, 0, .5, SLICES, STACKS);
	glPopMatrix();

	// right eye
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(EYE_OFFSET.x, EYE_OFFSET.y, EYE_OFFSET.z);
	gluSphere(quadric, EYE_RADIUS, SLICES, STACKS);
	glPopMatrix();

	// left eye
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(EYE_OFFSET.x, EYE_OFFSET.y, -EYE_OFFSET.z);
	gluSphere(quadric, EYE_RADIUS, SLICES, STACKS);
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
	// simply computing and adding each vertex manually
	glm::vec3 halfBounds = glm::vec3(BOUNDS.x / 2, BOUNDS.y / 2, BOUNDS.z / 2);

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

	// vertical lines
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
	return BOUNDS;
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