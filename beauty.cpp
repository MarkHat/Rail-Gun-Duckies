#include "beauty.h"

const int ROTATION_SPEED = 45;
const int VIEW_TILT = 5;

const glm::vec3 DUCKY_POSITION = glm::vec3(0, 1, -3);
const glm::vec3 GUN_POSITION = glm::vec3(0, 1, -5);
const glm::vec3 BALLOON_POSITION = glm::vec3(0, 3.3, -5);
const glm::vec3 PEDESTAL_POSITION = glm::vec3(0, -1.25, -7);

enum Mode
{
	DUCKY,
	RAIL_GUN,
	BALLOON
};

enum Mode Beauty::mode = DUCKY;

bool Beauty::debug = false;
double Beauty::oldElapsedTime = 0;
double Beauty::newElapsedTime = 0;
double Beauty::pausedTime = 0;
double Beauty::realElapsedTime = 0;

Ducky * Beauty::ducky = new Ducky();
RailGun * Beauty::railgun = new RailGun();
Balloon * Beauty::balloon = new Balloon();
Pedestal * Beauty::pedestal = new Pedestal();

void Beauty::ToggleDebug()
{
	debug = !debug;

	ducky->ToggleDebug();
	balloon->ToggleDebug();
	pedestal->ToggleDebug();
}

void Beauty::CycleMode()
{
	switch (mode)
	{
		case DUCKY:
			mode = RAIL_GUN;
			break;

		case RAIL_GUN:
			mode = BALLOON;
			break;

		case BALLOON:
			mode = DUCKY;
			break;
	}
}

void Beauty::DisplayXYZ()
{
	glPushMatrix();
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);

	glEnd();

	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Beauty::Update(bool paused)
{
	oldElapsedTime = newElapsedTime;
	newElapsedTime = (double) glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	double difference = newElapsedTime - oldElapsedTime;

	if (paused)
	{
		pausedTime += difference;
	}

	realElapsedTime = newElapsedTime - pausedTime;
}

void Beauty::Display()
{
	//glPushMatrix();
	//glRotatef(VIEW_TILT, 1, 0, 0);

	glPushMatrix();
	glTranslatef(PEDESTAL_POSITION.x, PEDESTAL_POSITION.y, PEDESTAL_POSITION.z);
	glRotatef(VIEW_TILT, 1, 0, 0);
	glRotatef(realElapsedTime * ROTATION_SPEED, 0, 1, 0);
	pedestal->Display();
	glPopMatrix();

	glPushMatrix();

	switch (mode)
	{
		case DUCKY:
			glTranslatef(DUCKY_POSITION.x, DUCKY_POSITION.y, DUCKY_POSITION.z);
			glRotatef(VIEW_TILT, 1, 0, 0);
			glRotatef(realElapsedTime * ROTATION_SPEED, 0, 1, 0);
			ducky->Display();

			break;
			
		case RAIL_GUN:
			glTranslatef(GUN_POSITION.x, GUN_POSITION.y, GUN_POSITION.z);
			glRotatef(VIEW_TILT, 1, 0, 0);
			glRotatef(realElapsedTime * ROTATION_SPEED, 0, 1, 0);
			railgun->Display();

			break;
			
		case BALLOON:
			glColor3f(1, 0, 0);
			glTranslatef(BALLOON_POSITION.x, BALLOON_POSITION.y, BALLOON_POSITION.z);
			glRotatef(VIEW_TILT, 1, 0, 0);
			glRotatef(realElapsedTime * ROTATION_SPEED, 0, 1, 0);
			balloon->Display();

			break;
	}

	if (debug)
	{
		DisplayXYZ();
	}

	glPopMatrix();

}