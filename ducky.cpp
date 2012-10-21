#include "ducky.h"

Ducky::Ducky()
{
	duckyDisplayList = -1;

	position = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
}

Ducky::~Ducky()
{
}

void Ducky::Display()
{
	if (duckyDisplayList == -1)
		{
			duckyDisplayList = glGenLists(1);

			glNewList(duckyDisplayList, GL_COMPILE);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glColor3f(1, 1, 0);

			// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluSphere.xml
			// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluNewQuadric.xml
			// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluDeleteQuadric.xml
			GLUquadric * quadric = gluNewQuadric();

			// reference: http://blog.repertoiremag.com/wp-content/uploads/2010/09/rubber-ducky1.jpg
			// body
			glPushMatrix();
			glScalef(2, 1, 1);
			gluSphere(quadric, .5, 25, 25);
			glPopMatrix();

			// left wing
			glPushMatrix();
			glTranslatef(0, 0.35, -0.85);
			glRotatef(10, 1, 0, 0);
			glScalef(0.6, 0.3, 0.2);
			gluSphere(quadric, 1, 25, 25);
			glPopMatrix();

			// right wing
			glPushMatrix();
			glTranslatef(0, 0.35, 0.85);
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

	glCallList(duckyDisplayList);
}

glm::vec3 Ducky::GetPosition()
{
	return position;
}

glm::vec3 Ducky::GetVelocity()
{
	return velocity;
}

void Ducky::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Ducky::SetVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void Ducky::Update(double elapsedTime, GLfloat gravity)
{
	position.x += velocity.x * elapsedTime;
	position.y += velocity.y * elapsedTime;
	position.z += velocity.z * elapsedTime;

	// apply gravity
	velocity.y += gravity;
}