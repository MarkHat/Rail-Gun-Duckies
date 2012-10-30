#include "railgun.h"

const int YAW_RANGE = 75;
const int PITCH_RANGE = 45;

RailGun::RailGun()
{
	railgunDisplayList = -1;

	yaw = 0;
	pitch = 0;

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

	// close end piece
	glPushMatrix();
	DrawEndPiece();
	glPopMatrix();

	// top-right long piece
	glPushMatrix();
	glTranslatef(0.67, 0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	// bottom-right long piece
	glPushMatrix();
	glTranslatef(0.67, -0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	// bottom-left long piece
	glPushMatrix();
	glTranslatef(-0.67, -0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();

	// top-left long piece
	glPushMatrix();
	glTranslatef(-0.67, 0.67, -5.5);
	DrawLongPiece();
	glPopMatrix();
	
	// far end piece
	glPushMatrix();
	glTranslatef(0, 0, -11);
	DrawEndPiece();
	glPopMatrix();

	glPopMatrix();
}

void RailGun::CreateRailgunDisplayList()
{
	railgunDisplayList = glGenLists(1);
	glNewList(railgunDisplayList, GL_COMPILE);

	glPushMatrix();
	glColor3f(1, 1, 1);

	// note: everything is scaled in half so that the gun's dimensions will be 1x1x6
	glScalef(0.5, 0.5, 0.5);
	DrawGun();
	glPopMatrix();

	glEndList();
}

void RailGun::Display()
{
	if (railgunDisplayList == -1)
	{
		CreateRailgunDisplayList();
	}

	glCallList(railgunDisplayList);
}

void RailGun::SetYaw(int yaw)
{
	this->yaw = yaw;
}

void RailGun::SetPitch(int pitch)
{
	this->pitch = pitch;
}

void RailGun::UpdateYaw(int mouseX, int windowWidth)
{
	yaw = -(mouseX - windowWidth / 2) / (windowWidth / YAW_RANGE);
}

void RailGun::UpdatePitch(int mouseY, int windowHeight)
{
	pitch = PITCH_RANGE - mouseY / (windowHeight / PITCH_RANGE);
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