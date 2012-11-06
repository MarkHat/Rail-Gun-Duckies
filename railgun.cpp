#include "railgun.h"

const int YAW_RANGE = 75;
const int PITCH_RANGE = 45;
const int ANIMATION_FRAMES = 60;

const glm::vec3 LONG_PIECE_SCALE = glm::vec3(0.33, 0.33, 5);
const glm::vec3 LONG_PIECE_TRANSLATION = glm::vec3(0.67, 0.67, -5.5);

const glm::vec3 END_PIECE_SCALE = glm::vec3(1, 1, 0.5);
const glm::vec3 FAR_END_PIECE_TRANSLATION = glm::vec3(0, 0, -11);

RailGun::RailGun()
{
	railgunDisplayList = -1;

	// note: most of these variables will be set by Game anyway
	yaw = 0;
	pitch = 0;
	targetYaw = 0;
	targetPitch = 0;
	position = glm::vec3(0, 0, 0);
	
	animating = false;
	animationFrame = 0;
}

RailGun::~RailGun()
{
}

void DrawFace()
{
	// this just draws a square
	glBegin(GL_QUADS);
	glVertex3f(1, 1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(-1, -1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
}

void DrawCube()
{
	// note: all faces are rotated so that they're normals face out

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
	glTranslatef(0, 1, 0);
	glRotatef(90, 1, 0, 0);
	DrawFace();
	glPopMatrix();
}

void DrawEndPiece()
{
	glScalef(END_PIECE_SCALE.x, END_PIECE_SCALE.y, END_PIECE_SCALE.z);
	DrawCube();
}

void DrawLongPiece()
{
	glScalef(LONG_PIECE_SCALE.x, LONG_PIECE_SCALE.y, LONG_PIECE_SCALE.z);
	DrawCube();
}

void DrawGun()
{
	glPushMatrix();

	// note: this translation causes the pivot point of the railgun to be in the right place 
	glTranslatef(0, 0, 4);

	// close end piece
	glPushMatrix();
	DrawEndPiece();
	glPopMatrix();

	// top-right long piece
	glPushMatrix();
	glTranslatef(LONG_PIECE_TRANSLATION.x, LONG_PIECE_TRANSLATION.y, LONG_PIECE_TRANSLATION.z);
	DrawLongPiece();
	glPopMatrix();

	// bottom-right long piece
	glPushMatrix();
	glTranslatef(LONG_PIECE_TRANSLATION.x, -LONG_PIECE_TRANSLATION.y, LONG_PIECE_TRANSLATION.z);
	DrawLongPiece();
	glPopMatrix();

	// bottom-left long piece
	glPushMatrix();
	glTranslatef(-LONG_PIECE_TRANSLATION.x, -LONG_PIECE_TRANSLATION.y, LONG_PIECE_TRANSLATION.z);
	DrawLongPiece();
	glPopMatrix();

	// top-left long piece
	glPushMatrix();
	glTranslatef(-LONG_PIECE_TRANSLATION.x, LONG_PIECE_TRANSLATION.y, LONG_PIECE_TRANSLATION.z);
	DrawLongPiece();
	glPopMatrix();
	
	// far end piece
	glPushMatrix();
	glTranslatef(FAR_END_PIECE_TRANSLATION.x, FAR_END_PIECE_TRANSLATION.y, FAR_END_PIECE_TRANSLATION.z);
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

void RailGun::SetYaw(GLfloat yaw)
{
	this->yaw = yaw;
}

void RailGun::SetPitch(GLfloat pitch)
{
	this->pitch = pitch;
}

void RailGun::SetTargetYaw(GLfloat targetYaw)
{
	this->targetYaw = targetYaw;
}

void RailGun::SetTargetPitch(GLfloat targetPitch)
{
	this->targetPitch = targetPitch;
}

void RailGun::UpdateYaw(int mouseX, int windowWidth)
{ 
	// the yaw is calculated from the horizontal center of the screen. YAW_RANGE is used
	// so that each screen half-width spans 0 - YAW_RANGE degrees
	yaw = -GLfloat((mouseX - windowWidth / 2)) / (windowWidth / YAW_RANGE);
}

void RailGun::UpdatePitch(int mouseY, int windowHeight)
{
	// unlike yaw, the pitch is calculated from the bottom of the screen. PITCH_RANGE is used
	// so that the entire height of the screen spans 0 - PITCH_RANGE degrees
	pitch = GLfloat(PITCH_RANGE) - mouseY / (windowHeight / PITCH_RANGE);
}

void RailGun::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void RailGun::SetAnimating()
{
	// these variables are used to allow the railgun to move smoothly from its current
	// pitch/yaw to its target pitch/yaw
	yawIncrement = (targetYaw - yaw) / ANIMATION_FRAMES;
	pitchIncrement = (targetPitch - pitch) / ANIMATION_FRAMES;

	animating = true;
}

void RailGun::MoveTowardsTarget()
{
	// the railgun will "animate" towards its target over ANIMATION_FRAMES frames
	if (animationFrame < ANIMATION_FRAMES)
	{
		yaw += yawIncrement;
		pitch += pitchIncrement;

		animationFrame++;
	}
	else
	{
		// reset animation
		animationFrame = 0;
		animating = false;

		// this corrects for any possible precision loss
		yaw = targetYaw;
		pitch = targetPitch;
	}
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

bool RailGun::IsAnimating()
{
	return animating;
}