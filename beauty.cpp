#include "beauty.h"

enum Mode
{
	DUCKY,
	RAIL_GUN,
	BALLOON,
	IDLE
};

enum Mode Beauty::mode = DUCKY;

bool Beauty::debug = false;
double Beauty::oldElapsedTime = 0;
double Beauty::newElapsedTime = 0;
double Beauty::pausedTime = 0;
double Beauty::realElapsedTime = 0;
int Beauty::rotationSpeed = 45;

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
			mode = IDLE;
			break;

		case IDLE:
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
	glPushMatrix();
	glRotatef(5, 1, 0, 0);

	glPushMatrix();
	glTranslatef(0, -2, -7);
	glRotatef(realElapsedTime * rotationSpeed, 0, 1, 0);
	pedestal->Display();
	glPopMatrix();

	glPushMatrix();

	switch (mode)
	{
		case DUCKY:
			glTranslatef(0, 1, -3);
			glRotatef(realElapsedTime * rotationSpeed, 0, 1, 0);
			ducky->Display();

			break;
			
		case RAIL_GUN:
			glTranslatef(0, 1, -5);
			glRotatef(realElapsedTime * rotationSpeed, 0, 1, 0);
			railgun->Display();

			break;
			
		case BALLOON:
			glTranslatef(0, 2.5, -5);
			glRotatef(realElapsedTime * rotationSpeed, 0, 1, 0);
			balloon->Display();

			break;
	}

	if (debug)
	{
		DisplayXYZ();
	}

	glPopMatrix();
	glPopMatrix();
}