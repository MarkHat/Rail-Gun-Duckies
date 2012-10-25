#include "game.h"

enum Mode
{
	MANUAL,
	AUTOMATED,
	IDLE
};

enum CameraModes
{
	FIXED_POSITION,
	REORIENT,
	FIRST_PERSON
};

int Game::windowWidth = 800;
int Game::windowHeight = 600;

glm::vec3 Game::duckyInitialPosition = glm::vec3(0, 0, -3);
glm::vec3 Game::gunInitialPosition = glm::vec3(0, -1, -3);
GLfloat Game::launchSpeed = 30;
GLfloat Game::radianConversion = 3.14159 / 180;
GLfloat Game::gravity = -0.2;

enum Mode Game::mode = IDLE;
enum CameraModes Game::cameraMode = FIXED_POSITION;

bool Game::debug = false;
bool Game::duckyFired = false;
double Game::oldElapsedTime = 0;
double Game::newElapsedTime = 0;

Ducky * Game::ducky = new Ducky();
RailGun * Game::railgun = new RailGun();

Balloon * Game::redBalloon = new Balloon();
Balloon * Game::greenBalloon = new Balloon();
Balloon * Game::blueBalloon = new Balloon();

void Game::ToggleDebug()
{
	debug = !debug;
}

void Game:: CycleCameraMode(){
	
	switch (cameraMode)
	{
		case FIXED_POSITION:
			cameraMode = REORIENT;
		
			break;

		case REORIENT:
			cameraMode = FIRST_PERSON;
			break;

		case FIRST_PERSON:
			cameraMode = FIXED_POSITION;
			gluLookAt(0, 0, 0, 0, 0, -5, 0, 1, 0);
			break;
	}
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
			break;

		case IDLE:
			mode = MANUAL;
			ResetGame();

			break;	
	}
}

void Game::MouseMotionFunc(int x, int y, int windowWidth, int windowHeight)
{
	if (!duckyFired)
	{
		railgun->UpdateYaw(x, windowWidth);
		railgun->UpdatePitch(y, windowHeight);
	}
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

void Game::GenerateBalloons()
{
	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat z = 0;

	int xOffset = 15;
	int yOffset = 5;
	int yHeight = 5;
	int zGunOffset = 25;
	int zBalloonOffset = 15;
	
	srand(time(NULL));

	for (int i = 0; i < 3; i++)
	{
		x = -xOffset + rand() % (xOffset * 2);
		y = yOffset + (rand() % yHeight);
		z = (zGunOffset + zBalloonOffset * i + rand() % zBalloonOffset) * -1;

		switch (i)
		{
			case 0:
				redBalloon->SetPosition(x, y, z);
				break;

			case 1:
				greenBalloon->SetPosition(x, y, z);
				break;

			case 2:
				blueBalloon->SetPosition(x, y, z);
				break;
		}
	}
}

void Game::ResetGame()
{
	ResetDucky();
	railgun->SetPosition(gunInitialPosition.x, gunInitialPosition.y, gunInitialPosition.z);

	GenerateBalloons();
}

void Game::ResetDucky()
{
	ducky->SetPosition(duckyInitialPosition.x, duckyInitialPosition.y, duckyInitialPosition.z);
	ducky->SetVelocity(0, 0, 0);

	duckyFired = false;
}

void Game::Update()
{
	oldElapsedTime = newElapsedTime;
	newElapsedTime = (double) glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	if (duckyFired)
	{
		double difference = newElapsedTime - oldElapsedTime;

		ducky->Update(difference, gravity);
	}
}

void Game::DisplayDucky()
{
	glm::vec3 duckyPosition = ducky->GetPosition();
	glm::vec3 duckyVelocity = ducky->GetVelocity();

	if (duckyPosition.z < -80) {
		ResetDucky();
	}

	glPushMatrix();
	glTranslatef(duckyPosition.x, duckyPosition.y, duckyPosition.z);
	glRotatef(90, 0, 1, 0);

	if (!duckyFired)
	{
		glRotatef(railgun->GetPitch(), 0, 0, 1);
		glRotatef(railgun->GetYaw(), 0, 1, 0);
	}

	else
	{
		GLfloat angleX = -glm::asin(duckyVelocity.x / launchSpeed) / radianConversion;
		GLfloat angleY = glm::asin(duckyVelocity.y / launchSpeed) / radianConversion;
		GLfloat angleZ = -glm::acos(duckyVelocity.z / launchSpeed) / radianConversion;

		glRotatef(angleX, 0, 1, 0);
		glRotatef(angleY, 0, 0, 1);
		//glRotatef(angleZ, 1, 0, 0);
	}
	
	glTranslatef(-2, 0, 0);
	ducky->Display();
	glPopMatrix();
}

void Game::DisplayRailGun()
{
	glm::vec3 railGunPosition = railgun->GetPosition();

	glPushMatrix();
	glTranslatef(railGunPosition.x, railGunPosition.y, railGunPosition.z);
	glRotatef(railgun->GetPitch(), 1, 0, 0);
	glRotatef(railgun->GetYaw(), 0, 1, 0);
	railgun->Display();
	glPopMatrix();
}

void Game::DisplayBalloons()
{
	glm::vec3 redPosition = redBalloon->GetPosition();
	glm::vec3 greenPosition = greenBalloon->GetPosition();
	glm::vec3 bluePosition = blueBalloon->GetPosition();

	glPushMatrix();
	glTranslatef(redPosition.x, redPosition.y, redPosition.z);
	glColor3f(1, 0, 0);
	redBalloon->Display();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(greenPosition.x, greenPosition.y, greenPosition.z);
	glColor3f(0, 1, 0);
	greenBalloon->Display();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(bluePosition.x, bluePosition.y, bluePosition.z);
	glColor3f(0, 0, 1);
	blueBalloon->Display();
	glPopMatrix();
}

void Game::DisplayText(char * text)
{
	glPushMatrix();
	glScaled(0.01, 0.01, 0.01);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) text);
	glPopMatrix();
}

void Game::DisplayBalloonText()
{
	glm::vec3 redPosition = redBalloon->GetPosition();
	glm::vec3 greenPosition = greenBalloon->GetPosition();
	glm::vec3 bluePosition = blueBalloon->GetPosition();

	float radiusMultiplier = 1.25;
	float redTextHalfWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, '1') * 0.005;
	float greenTextHalfWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, '2') * 0.005;
	float blueTextHalfWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, '3') * 0.005;

	glPushMatrix();
	glColor3f(1, 1, 1);

	glPushMatrix();
	glTranslatef(redPosition.x - redTextHalfWidth, redPosition.y + redBalloon->GetRadius() * radiusMultiplier, redPosition.z);
	DisplayText("1");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(greenPosition.x - greenTextHalfWidth, greenPosition.y + greenBalloon->GetRadius() * radiusMultiplier, greenPosition.z);
	DisplayText("2");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(bluePosition.x - blueTextHalfWidth, bluePosition.y + blueBalloon->GetRadius() * radiusMultiplier, bluePosition.z);
	DisplayText("3");
	glPopMatrix();
	
	glPopMatrix();
}

void Game::SetWindowDimensions(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
}

void Game::DisplayGameInfo()
{
	//char * scoreText = strcat("Score: ", "0");
	//char * duckiesText = strcat("Duckies left: ", "3");
	
	char * scoreText = "Score: 0";
	char * duckiesText = "Duckies left: 3";

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 1, 10);
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glColor3f(1, 1, 1);

	glPushMatrix();
	glTranslatef(10, 10, -5.5);
	glScalef(0.15, 0.15, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) duckiesText);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, glutStrokeHeight(GLUT_STROKE_MONO_ROMAN) * 0.15 + 20, -5.5);
	glScalef(0.15, 0.15, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) scoreText);
	glPopMatrix();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void Game::Display()
{	
	glPushMatrix();
	
	if (cameraMode == FIRST_PERSON) {
		glm::vec3 duckyPosition = ducky->GetPosition();

		gluLookAt(duckyPosition.x, duckyPosition.y, duckyPosition.z, duckyPosition.x, duckyPosition.y, duckyPosition.z - .5, 0, 1, 0);
	}

	else if (cameraMode == REORIENT) {
		gluLookAt(0, 0, 2, 0, 0, -5, 0, 1, 0);
	}

	DisplayDucky();
	DisplayRailGun();
	DisplayBalloons();
	DisplayBalloonText();
	DisplayGameInfo();
	
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

		GLfloat velocityX = -glm::sin(railgun->GetYaw() * radianConversion) * launchSpeed;
		GLfloat velocityY = glm::sin(railgun->GetPitch() * radianConversion) * launchSpeed;
		GLfloat velocityZ = -glm::cos(railgun->GetPitch() * radianConversion) * launchSpeed;

		ducky->SetVelocity(velocityX, velocityY, velocityZ);
	}
	else
	{
		ResetDucky();
	}
}