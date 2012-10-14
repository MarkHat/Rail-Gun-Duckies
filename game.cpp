#include "game.h"

enum Mode
{
	MANUAL,
	AUTOMATED,
	IDLE
};

enum Mode Game::mode = IDLE;

bool Game::debug = false;
bool Game::duckyFired = false;
double Game::oldElapsedTime = 0;
double Game::newElapsedTime = 0;

Ducky * Game::ducky = new Ducky();
RailGun * Game::railgun = new RailGun();
Balloon * Game::balloon = new Balloon();

void Game::ToggleDebug()
{
	debug = !debug;
}

void Game::CycleMode()
{
	switch (mode)
	{
		case MANUAL:
			mode = AUTOMATED;
			ResetGame();

			break;

		case AUTOMATED:
			mode = IDLE;
			ResetGame();

			break;

		case IDLE:
			mode = MANUAL;
			break;	
	}
}

void Game::MouseMotionFunc(int x, int y, int windowWidth, int windowHeight)
{
	railgun->UpdateYaw(x, windowWidth);
	railgun->UpdatePitch(y, windowHeight);
}

void Game::DisplayXYZ()
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

void Game::ResetGame()
{
	ResetDucky();
}

void Game::ResetDucky()
{
	ducky->SetPosition(-1.5, 1, 0);
}

void Game::Update()
{
	oldElapsedTime = newElapsedTime;
	newElapsedTime = (double) glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	double difference = newElapsedTime - oldElapsedTime;

	ducky->Update(difference);
}

void Game::DisplayDucky()
{
	/*
	glm::vec3 duckyPosition = ducky->GetPosition();

	glPushMatrix();
	glTranslatef(duckyPosition.x, duckyPosition.y, duckyPosition.z);
	glPopMatrix();
	*/
}

void Game::Display()
{
	glPushMatrix();

	switch (mode)
	{
		case MANUAL:
			glPushMatrix();
			glTranslatef(0, 0, -8);
			glRotatef(railgun->GetPitch(), 1, 0, 0);
			glRotatef(railgun->GetYaw(), 0, 1, 0);
			railgun->Display();
			glPopMatrix();

			DisplayDucky();

			break;
			
		case AUTOMATED:
			break;
	}

	if (debug)
	{
		DisplayXYZ();
	}

	glPopMatrix();
}

void Game::FireDucky()
{
	if (!duckyFired)
	{
		duckyFired = true;
		ducky->SetVelocity(0, 0, 1);
	}
}