#include "railgun.h"

RailGun::RailGun()
{
	railgunDisplayList = -1;

	yaw = 0;
	pitch = 0;
	yawRange = 75;
	pitchRange = 45;

	position = glm::vec3(0, 0, 0);
}

RailGun::~RailGun()
{
}

void DrawFace()
{
	glBegin(GL_QUADS);
	glVertex3f(1, 1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(-1, -1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
}

void DrawCube()
{
	// back face
	glPushMatrix();
	glTranslatef(0, 0, -1);
	DrawFace();
	glPopMatrix();

	// front face
	glPushMatrix();
	glTranslatef(0, 0, 1);
	glRotatef(180, 0, 1, 0);
	DrawFace();
	glPopMatrix();

	// left face
	glPushMatrix();
	glTranslatef(-1, 0, 0);
	glRotatef(90, 0, 1, 0);
	DrawFace();
	glPopMatrix();

	// right face
	glPushMatrix();
	glTranslatef(1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	DrawFace();
	glPopMatrix();

	// bottom face
	glPushMatrix();
	glTranslatef(0, -1, 0);
	glRotatef(-90, 1, 0, 0);
	DrawFace();
	glPopMatrix();

	// top face
	glPushMatrix();
	glTranslatef(0, 1., 0);
	glRotatef(90, 1, 0, 0);
	DrawFace();
	glPopMatrix();
}

void DrawEndPiece()
{
	glScalef(1, 1, 0.5);
	DrawCube();
}

void DrawLongPiece()
{
	glScalef(0.33, 0.33, 5);
	DrawCube();
}

void DrawGun()
{
	glPushMatrix();
	glTranslatef(0, 0, 4);

	glPushMatrix();
	DrawEndPiece();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.67, 0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.67, -0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.67, -0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.67, 0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, 0, -11);
	DrawEndPiece();
	glPopMatrix();

	glPopMatrix();
}

void RailGun::Display()
{
	if (railgunDisplayList == -1)
	{
		railgunDisplayList = glGenLists(1);
		glNewList(railgunDisplayList, GL_COMPILE);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glScalef(0.5, 0.5, 0.5);
		DrawGun();
		glPopMatrix();
		glEndList();
	}

	glCallList(railgunDisplayList);
}

void RailGun::UpdateYaw(int mouseX, int windowWidth)
{
	yaw = -(mouseX - windowWidth / 2) / (windowWidth / yawRange);
}

void RailGun::UpdatePitch(int mouseY, int windowHeight)
{
	pitch = pitchRange - mouseY / (windowHeight / pitchRange);
}

void RailGun::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

float RailGun::GetYaw()
{
	return yaw;
}

float RailGun::GetPitch()
{
	return pitch;
}

glm::vec3 RailGun::GetPosition()
{
	return position;
}