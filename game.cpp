#include "game.h"

const int FPS = 60;

const char * SCORE_TEXT = "Score: ";
const char * DUCKIES_LEFT_TEXT = "Duckies left: ";
const char * CAMERA_MODE_TEXT = "Camera mode: ";

const glm::vec3 SCORE_TEXT_POSITION = glm::vec3(10, 35, -1);
const glm::vec3 DUCKIES_LEFT_TEXT_POSITION = glm::vec3(10, 10, -1);
const glm::vec3 CAMERA_MODE_TEXT_POSITION = glm::vec3(10, 85, -1);

const GLfloat SCORE_TEXT_SCALE = GLfloat(0.12);
const GLfloat DUCKIES_LEFT_TEXT_SCALE = GLfloat(0.12);
const GLfloat CAMERA_MODE_TEXT_SCALE = GLfloat(0.12);
const GLfloat BALLOON_TEXT_SCALE = GLfloat(0.01);

const GLfloat LAUNCH_SPEED = 30;
const GLfloat RADIAN_CONVERSION = GLfloat(3.14159) / GLfloat(180.0);
const GLfloat GRAVITY = GLfloat(-0.2);

const glm::vec3 DUCKY_INITIAL_POSITION = glm::vec3(0, 0, -4);
const glm::vec3 GUN_INITIAL_POSITION = glm::vec3(0, -1, -4);

const int BALLOON_GEN_X_OFFSET = 15;
const int BALLOON_GEN_Y_OFFSET = 5;
const int BALLOON_GEN_Y_HEIGHT = 5;
const int BALLOON_GEN_Z_OFFSET = 25;
const int BALLOON_GEN_Z_LENGTH = 15;

const int COLLISION_FAR_BOUNDARY = -80;
const int COLLISION_TIME_SEGMENTS = 10;

const int RED_BALLOON_VALUE = 1;
const int GREEN_BALLOON_VALUE = 2;
const int BLUE_BALLOON_VALUE = 3;
const GLfloat BALLOON_TEXT_RADIUS_MULTIPLIER = 1.25;

const char * RED_BALLOON_VALUE_TEXT = "1";
const char * GREEN_BALLOON_VALUE_TEXT = "2";
const char * BLUE_BALLOON_VALUE_TEXT = "3";

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
	THIRD_PERSON,
	FIRST_PERSON,
};

enum ReplayType
{
	NEW_GAME,
	MISS,
	RED_BALLOON,
	GREEN_BALLOON,
	BLUE_BALLOON,
	DUCKY_FIRED_MID_FLIGHT
};

int Game::score = 0;
int Game::duckiesLeft = 3;

int Game::windowWidth = 800;
int Game::windowHeight = 600;

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

bool Game::redHit = false;
bool Game::greenHit = false;
bool Game::blueHit = false;

glm::vec3 Game::cameraPosition = glm::vec3(0, 0, 0);
glm::vec3 Game::cameraLookAt = glm::vec3(0, 0, -1);

double Game::prevPitch = 0;
double Game::prevYaw = 0;
int Game::prevScore;
int Game::prevDuckiesLeft;
enum ReplayType Game::replayType = NEW_GAME;
bool Game::replay = FALSE;
double Game::replayTimer = 0;
glm::vec3 Game::prevRedBalloonPosition;
glm::vec3 Game::prevGreenBalloonPosition;
glm::vec3 Game::prevBlueBalloonPosition;
glm::vec3 Game::currRedBalloonPosition;
glm::vec3 Game::currGreenBalloonPosition;
glm::vec3 Game::currBlueBalloonPosition;
glm::vec3 Game::duckyReFiredPosition;
bool Game::prevRedHit = FALSE;
bool Game::prevBlueHit = FALSE;
bool Game::prevGreenHit = FALSE;

bool Game::paused = false;

void Game::Destruct()
{
	delete(ducky);
	delete(railgun);
	delete(redBalloon);
	delete(greenBalloon);
	delete(blueBalloon);
}

void Game::TogglePause()
{
	paused = !paused;
}

void Game::ToggleDebug()
{
	debug = !debug;

	ducky->ToggleDebug();
	redBalloon->ToggleDebug();
	greenBalloon->ToggleDebug();
	blueBalloon->ToggleDebug();
}

void Game::CycleCameraMode()
{
	switch (cameraMode)
	{
		case FIXED_POSITION:
			cameraMode = REORIENT;
			break;

		case REORIENT:
			cameraMode = THIRD_PERSON;
			break;

		case THIRD_PERSON:
			cameraMode = FIRST_PERSON;
			break;

		case FIRST_PERSON:
			cameraMode = FIXED_POSITION;
			break;
	}
}

void Game::CycleGameMode()
{
	switch (mode)
	{
		case MANUAL:
			mode = AUTOMATED;
			ToggleReplay();
			replayType = NEW_GAME;
			ResetGame();

			break;

		case AUTOMATED:
			mode = IDLE;
			break;

		case IDLE:
			mode = MANUAL;
			replayType = NEW_GAME;
			ResetGame();

			break;	
	}
}

void Game::MouseMotionFunc(int x, int y, int windowWidth, int windowHeight)
{
	if (!duckyFired && !replay)
	{
		railgun->UpdateYaw(x, windowWidth);
		railgun->UpdatePitch(y, windowHeight);
	}
}

void Game::DisplayOrthoText(const char * text, glm::vec3 position, GLfloat scale)
{
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glScalef(scale, scale, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) text);
	glPopMatrix();
}

void Game::DisplayBalloonText(Balloon * balloon, const char * balloonText)
{
	glm::vec3 balloonPosition = balloon->GetPosition();

	// position the text
	// note: the text half-width is calculated here because it wasn't working as a constant
	GLfloat textHalfWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, 'a') * BALLOON_TEXT_SCALE / 2;

	glPushMatrix();
	glTranslatef(balloonPosition.x - textHalfWidth,
				 balloonPosition.y + redBalloon->GetRadius() * BALLOON_TEXT_RADIUS_MULTIPLIER,
				 balloonPosition.z);

	// orient the text to face the camera
	float xDifference = balloonPosition.x - cameraPosition.x;
	float zDifference = balloonPosition.z - cameraPosition.z;
	float angle = atan(xDifference / zDifference) / RADIAN_CONVERSION;

	glRotatef(angle, 0, 1, 0);

	// scale and draw the text
	glScaled(BALLOON_TEXT_SCALE, BALLOON_TEXT_SCALE, BALLOON_TEXT_SCALE);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) balloonText);
	glPopMatrix();
}

void Game::RegenerateBalloons()
{
	// note: the casts to GLfloat get rid of a few warnings
	if (replay) 
	{
		redBalloon->SetPosition(currRedBalloonPosition.x, currRedBalloonPosition.y, currRedBalloonPosition.z);
		greenBalloon->SetPosition(currGreenBalloonPosition.x, currGreenBalloonPosition.y, currGreenBalloonPosition.z);
		blueBalloon->SetPosition(currBlueBalloonPosition.x, currBlueBalloonPosition.y, currBlueBalloonPosition.z);
	}

	else
	{
		srand((unsigned int)time(NULL));

		for (int i = 0; i < 3; i++)
		{
			GLfloat x = GLfloat(-BALLOON_GEN_X_OFFSET) + rand() % (BALLOON_GEN_X_OFFSET * 2);
			GLfloat y = GLfloat(BALLOON_GEN_Y_OFFSET) + (rand() % BALLOON_GEN_Y_HEIGHT);
			GLfloat z = (GLfloat(BALLOON_GEN_Z_OFFSET) + BALLOON_GEN_Z_LENGTH * i + rand() % BALLOON_GEN_Z_LENGTH) * -1;

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
}

void Game::ResetGame()
{
	ResetDucky();
	railgun->SetPosition(GUN_INITIAL_POSITION.x, GUN_INITIAL_POSITION.y, GUN_INITIAL_POSITION.z);
	ResetBalloons();

	score = 0;
	duckiesLeft = 3;
}

void Game::ResetDucky()
{
	ducky->SetPosition(DUCKY_INITIAL_POSITION.x, DUCKY_INITIAL_POSITION.y, DUCKY_INITIAL_POSITION.z);
	ducky->SetVelocity(0, 0, 0);

	duckyFired = false;
}

void Game::ResetBalloons()
{
	prevRedBalloonPosition = redBalloon->GetPosition();
	prevGreenBalloonPosition = greenBalloon->GetPosition();
	prevBlueBalloonPosition = blueBalloon->GetPosition();

	RegenerateBalloons();

	redHit = false;
	greenHit = false;
	blueHit = false;
}

bool Game::CheckDuckyBalloonCollision(glm::vec3 duckyTempPosition, Balloon * balloon) 
{
	glm::vec3 balloonPosition = balloon->GetPosition();

	float xDifference = glm::abs(balloonPosition.x - duckyTempPosition.x);
	float yDifference = glm::abs(balloonPosition.y - duckyTempPosition.y);
	float zDifference = glm::abs(balloonPosition.z - duckyTempPosition.z);

	glm::vec3 duckyBounds = ducky->GetBounds();
	glm::vec3 balloonBounds = balloon->GetBounds();

	if (xDifference < duckyBounds.x / 2 + balloonBounds.x / 2 &&
		yDifference < duckyBounds.y / 2 + balloonBounds.y / 2 &&
		zDifference < duckyBounds.z / 2 + balloonBounds.z / 2)
	{
		return true;
	}

	return false;
}

void Game::HandleCollisions()
{
	// destroy the ducky when it passes a certain z distance
	if (ducky->GetNewPosition().z < COLLISION_FAR_BOUNDARY) {
		
		SetPreviousGameState(MISS);
		duckiesLeft--;

		if (duckiesLeft == 0)
		{
			ResetGame();
		}
		else
		{
			ResetDucky();
		}
	}
	else
	{
		glm::vec3 duckyOldPosition = ducky->GetOldPosition();
		glm::vec3 duckyNewPosition = ducky->GetNewPosition();

		float xIncrement = (duckyNewPosition.x - duckyOldPosition.x) / COLLISION_TIME_SEGMENTS;
		float zIncrement = (duckyNewPosition.z - duckyOldPosition.z) / COLLISION_TIME_SEGMENTS;

		glm::vec3 duckyTempPosition = duckyOldPosition;

		for (int i = 0; i < COLLISION_TIME_SEGMENTS; i++)
		{
			duckyTempPosition.x += xIncrement;
			duckyTempPosition.z += zIncrement;

			if (!redHit && CheckDuckyBalloonCollision(duckyTempPosition, redBalloon))
			{
				SetPreviousGameState(RED_BALLOON);
				score += RED_BALLOON_VALUE;
				redHit = true;

				ResetDucky();
			}
			else if (!greenHit && CheckDuckyBalloonCollision(duckyTempPosition, greenBalloon))
			{
				SetPreviousGameState(GREEN_BALLOON);
				score += GREEN_BALLOON_VALUE;
				greenHit = true;

				ResetDucky();
			}
			else if (!blueHit && CheckDuckyBalloonCollision(duckyTempPosition, blueBalloon))
			{
				SetPreviousGameState(BLUE_BALLOON);
				score += BLUE_BALLOON_VALUE;
				blueHit = true;

				ResetDucky();
			}
		}
	}
}

GLfloat Game::EstimateTargetPitch(float xDifference, float yDifference, float zDifference, float targetY)
{
	float topDownDistance = glm::sqrt(xDifference * xDifference + zDifference * zDifference);
	float duckyInitialY = ducky->GetNewPosition().y;

	// initially, just point the gun straight at the target balloon
	GLfloat targetPitch = glm::atan(yDifference / topDownDistance) / RADIAN_CONVERSION;
	
	float verticalSpeed = 0;
	float horizontalSpeed = 0;
	int framesToTarget = 0;
	float duckyYAtTarget = 0;
	
	// this loop calculates the 
	while (duckyYAtTarget < targetY)
	{
		// note: the velocities here are per frame rather than per second
		verticalSpeed = glm::sin(targetPitch * RADIAN_CONVERSION) * LAUNCH_SPEED / 60;
		horizontalSpeed = glm::cos(targetPitch * RADIAN_CONVERSION) * LAUNCH_SPEED / 60;
		framesToTarget = int(topDownDistance / horizontalSpeed);
		duckyYAtTarget = duckyInitialY;

		for (int i = 0; i < framesToTarget; i++)
		{
			duckyYAtTarget += verticalSpeed;
			verticalSpeed += GRAVITY / FPS;
		}

		targetPitch++;
	}

	// note: this formula was giving us too small a pitch
	// reference: http://en.wikipedia.org/wiki/Trajectory_of_a_projectile

	/*
	float distance = sqrt(xDifference * xDifference + zDifference * zDifference);
	float targetPitch = atan((LAUNCH_SPEED * LAUNCH_SPEED -
							  sqrt(LAUNCH_SPEED * LAUNCH_SPEED * LAUNCH_SPEED * LAUNCH_SPEED - 
								   GRAVITY * (GRAVITY * distance * distance + 
											  2 * yDifference * LAUNCH_SPEED * LAUNCH_SPEED))) /
							  (GRAVITY * distance));

	return targetPitch / RADIAN_CONVERSION;
	*/

	return targetPitch;
}

void Game::AutomateRailgun()
{
	// if the railgun hasn't acquired a target, compute its target yaw and pitch
	if (!railgun->IsAnimating())
	{
		glm::vec3 railgunPosition = railgun->GetPosition();
		glm::vec3 targetPosition;

		if (!redHit)
		{
			targetPosition = redBalloon->GetPosition();
		}
		else if (!greenHit)
		{
			targetPosition = greenBalloon->GetPosition();
		}
		else if (!blueHit)
		{
			targetPosition = blueBalloon->GetPosition();
		}

		float xDifference = targetPosition.x - railgunPosition.x;
		float yDifference = targetPosition.y - railgunPosition.y;
		float zDifference = targetPosition.z - railgunPosition.z;

		glm::vec3 directVector = glm::vec3(xDifference, 0, zDifference);
		glm::vec3 zVector = glm::vec3(0, 0, zDifference);

		GLfloat targetYaw = glm::atan(xDifference / zDifference) / RADIAN_CONVERSION;

		// reference: http://en.wikipedia.org/wiki/Trajectory_of_a_projectile
		GLfloat targetPitch = EstimateTargetPitch(xDifference, yDifference, zDifference, targetPosition.y);

		railgun->SetTargetYaw(targetYaw);
		railgun->SetTargetPitch(targetPitch);

		railgun->SetAnimating();
	}
	// if the railgun has a target, animate it
	else
	{
		railgun->MoveTowardsTarget();

		// check if the railgun has finished animating
		if (!railgun->IsAnimating())
		{
			FireDucky();
		}
	}
}

void Game::Update()
{
	oldElapsedTime = newElapsedTime;
	newElapsedTime = double(glutGet(GLUT_ELAPSED_TIME)) / 1000;

	double difference = newElapsedTime - oldElapsedTime;

	if (replay && !duckyFired)
	{
		replayTimer++;

		if (replayTimer == 60)
		{
			replayTimer = 0;
			Replay();
		}
	}
	else if (!paused){
		if (mode == AUTOMATED)
		{
			if (!duckyFired)
			{
				AutomateRailgun();
			}
		}

		if (duckyFired)
		{
			ducky->Update(difference, LAUNCH_SPEED, GRAVITY);
			HandleCollisions();

			
				if (replay && replayType == DUCKY_FIRED_MID_FLIGHT)
				{
					glm::vec3 duckyPosition = ducky->GetNewPosition();

					if (duckyPosition.z <= duckyReFiredPosition.z)
					{
						FireDucky();
					}
				}

			// check if new balloons should be generated
			if (redHit && greenHit && blueHit)
			{
				ResetBalloons();
			}
		}
	}
}

void Game::DisplayDucky()
{
	glm::vec3 duckyPosition = ducky->GetNewPosition();
	glm::vec3 duckyVelocity = ducky->GetVelocity();

	glPushMatrix();
	glTranslatef(duckyPosition.x, duckyPosition.y, duckyPosition.z);
	glRotatef(90, 0, 1, 0);

	if (!duckyFired)
	{
		glRotatef(railgun->GetYaw(), 0, 1, 0);
		glRotatef(railgun->GetPitch(), 0, 0, 1);
	}
	else
	{
		glm::vec3 duckyRotation = ducky->GetRotation();

		// note: x and z are reversed here because the ducky has been rotated 90 degrees to face forward
		glRotatef(duckyRotation.x, 0, 1, 0);
		glRotatef(duckyRotation.y, 0, 0, 1);
		glRotatef(duckyRotation.z, 1, 0, 0);
	}
	
	// move the ducky to the end of the gun
	glTranslatef(-2, 0, 0);

	ducky->Display();
	glPopMatrix();
}

void Game::DisplayRailGun()
{
	glm::vec3 railGunPosition = railgun->GetPosition();

	glPushMatrix();
	glTranslatef(railGunPosition.x, railGunPosition.y, railGunPosition.z);
	glRotatef(railgun->GetYaw(), 0, 1, 0);
	glRotatef(railgun->GetPitch(), 1, 0, 0);
	railgun->Display();
	glPopMatrix();
}

void Game::DisplayBalloons()
{
	glm::vec3 redPosition = redBalloon->GetPosition();
	glm::vec3 greenPosition = greenBalloon->GetPosition();
	glm::vec3 bluePosition = blueBalloon->GetPosition();

	if (!redHit)
	{
		glPushMatrix();
		glTranslatef(redPosition.x, redPosition.y, redPosition.z);
		glColor3f(1, 0, 0);
		redBalloon->Display();
		glPopMatrix();
	}

	if (!greenHit)
	{
		glPushMatrix();
		glTranslatef(greenPosition.x, greenPosition.y, greenPosition.z);
		glColor3f(0, 1, 0);
		greenBalloon->Display();
		glPopMatrix();
	}

	if (!blueHit)
	{
		glPushMatrix();
		glTranslatef(bluePosition.x, bluePosition.y, bluePosition.z);
		glColor3f(0, 0, 1);
		blueBalloon->Display();
		glPopMatrix();
	}
}

void Game::DisplayBalloonValues()
{
	glm::vec3 redPosition = redBalloon->GetPosition();
	glm::vec3 greenPosition = greenBalloon->GetPosition();
	glm::vec3 bluePosition = blueBalloon->GetPosition();

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	if (!redHit)
	{
		DisplayBalloonText(redBalloon, RED_BALLOON_VALUE_TEXT);
	}

	if (!greenHit)
	{
		DisplayBalloonText(greenBalloon, GREEN_BALLOON_VALUE_TEXT);
	}

	if (!blueHit)
	{
		DisplayBalloonText(blueBalloon, BLUE_BALLOON_VALUE_TEXT);
	}
	
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Game::SetWindowDimensions(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
}

char * Game::AssignCameraModeText()
{
	char * text;

	switch (cameraMode)
	{
		case FIXED_POSITION:
			text = "Fixed";
			break;

		case REORIENT:
			text = "Reorient";
			break;

		case THIRD_PERSON:
			text = "Third Person";
			break;

		case FIRST_PERSON:
			text = "First Person";
			break;
	}

	return text;
}

void Game::DisplayScore()
{
	DisplayOrthoText(SCORE_TEXT, SCORE_TEXT_POSITION, SCORE_TEXT_SCALE);
	
	// display the actual value
	// note: this is easiest way I could find to convert an integer to a string
	char scoreValueText[5];
	sprintf_s(scoreValueText, "%d", score);

	// note: I'm calculating the text width here because it wasn't working as a constant
	GLfloat scoreTextWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, 'a') * SCORE_TEXT_SCALE * strlen(SCORE_TEXT);
	//char * scoreValueText = ConvertToString(score);
	glm::vec3 scoreValuePosition = glm::vec3(SCORE_TEXT_POSITION.x + scoreTextWidth,
											 SCORE_TEXT_POSITION.y, SCORE_TEXT_POSITION.z);

	DisplayOrthoText(scoreValueText, scoreValuePosition, SCORE_TEXT_SCALE);
}

void Game::DisplayDuckiesLeft()
{
	DisplayOrthoText(DUCKIES_LEFT_TEXT, DUCKIES_LEFT_TEXT_POSITION, DUCKIES_LEFT_TEXT_SCALE);
	
	// display the actual value
	// note: this is easiest way I could find to convert an integer to a string
	char duckiesLeftValueText[2];
	sprintf_s(duckiesLeftValueText, "%d", duckiesLeft);

	// note: I'm calculating the text width here because it wasn't working as a constant
	GLfloat duckiesLeftTextWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, 'a') * DUCKIES_LEFT_TEXT_SCALE *
							   strlen(DUCKIES_LEFT_TEXT);
	glm::vec3 duckiesLeftValuePosition = glm::vec3(DUCKIES_LEFT_TEXT_POSITION.x + duckiesLeftTextWidth,
												   DUCKIES_LEFT_TEXT_POSITION.y, DUCKIES_LEFT_TEXT_POSITION.z);

	DisplayOrthoText(duckiesLeftValueText, duckiesLeftValuePosition, SCORE_TEXT_SCALE);
}

void Game::DisplayCameraMode()
{
	DisplayOrthoText(CAMERA_MODE_TEXT, CAMERA_MODE_TEXT_POSITION, CAMERA_MODE_TEXT_SCALE);
	
	// display the actual value
	// note: I'm calculating the text width here because it wasn't working as a constant
	GLfloat cameraModeTextWidth = glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, 'a') * CAMERA_MODE_TEXT_SCALE *
							  strlen(CAMERA_MODE_TEXT);
	char * cameraModeValueText = AssignCameraModeText();
	glm::vec3 cameraModeValuePosition = glm::vec3(CAMERA_MODE_TEXT_POSITION.x + cameraModeTextWidth,
												  CAMERA_MODE_TEXT_POSITION.y, CAMERA_MODE_TEXT_POSITION.z);

	DisplayOrthoText(cameraModeValueText, cameraModeValuePosition, CAMERA_MODE_TEXT_SCALE);
}

void Game::DisplayGameInfo()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 1, 10);
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glDisable(GL_LIGHTING);

	glLoadIdentity();
	glColor3f(1, 1, 1);

	DisplayScore();
	DisplayDuckiesLeft();
	DisplayCameraMode();

	glEnable(GL_LIGHTING);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void Game::SetCamera()
{	
	glm::vec3 duckyPosition = ducky->GetNewPosition();
	glm::vec3 duckyHeadPosition = ducky->GetHeadPosition();

	switch (cameraMode)
	{
		case FIXED_POSITION:
			cameraPosition.x = 0;
			cameraPosition.y = 0;
			cameraPosition.z = 0;

			cameraLookAt.x = 0;
			cameraLookAt.y = 0;
			cameraLookAt.z = -1;

			break;

		case REORIENT:
			cameraPosition.x = 0;
			cameraPosition.y = 0;
			cameraPosition.z = 0;

			cameraLookAt.x = duckyPosition.x;
			cameraLookAt.y = duckyPosition.y;
			cameraLookAt.z = duckyPosition.z;

			break;

		case THIRD_PERSON:
			cameraPosition.x = duckyPosition.x;
			cameraPosition.y = duckyPosition.y;
			cameraPosition.z = duckyPosition.z + GLfloat(0.5);

			cameraLookAt.x = duckyPosition.x;
			cameraLookAt.y = duckyPosition.y;
			cameraLookAt.z = duckyPosition.z;

			break;

		case FIRST_PERSON:
			cameraPosition.x = duckyHeadPosition.x;
			cameraPosition.y = duckyHeadPosition.y;
			cameraPosition.z = duckyHeadPosition.z;

			glm::vec3 duckyVelocity = ducky->GetVelocity();

			// check if the ducky is sitting on the railgun
			if (!duckyFired)
			{
				// reorient the camera to face the end of the gun
				cameraLookAt.x = duckyHeadPosition.x + -sin(railgun->GetYaw() * RADIAN_CONVERSION);
				cameraLookAt.y = duckyHeadPosition.y + sin(railgun->GetPitch() * RADIAN_CONVERSION);
				cameraLookAt.z = duckyHeadPosition.z + -cos(railgun->GetYaw() * RADIAN_CONVERSION);
			}
			else
			{
				// reorient the camera to face tangent to the ducky's flight path
				cameraLookAt.x = duckyHeadPosition.x + duckyVelocity.x;
				cameraLookAt.y = duckyHeadPosition.y + duckyVelocity.y;
				cameraLookAt.z = duckyHeadPosition.z + duckyVelocity.z;
			}

			break;
	}

	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
			  cameraLookAt.x, cameraLookAt.y, cameraLookAt.z,
			  0, 1, 0);
}

void Game::Display()
{	
	glPushMatrix();

	SetCamera();

	// in first-person, the ducky isn't drawn
	if (cameraMode != FIRST_PERSON)
	{
		DisplayDucky();
	}

	DisplayRailGun();
	DisplayBalloons();
	DisplayBalloonValues();
	DisplayGameInfo();

	glPopMatrix();
}

void Game::FireDucky()
{
	if (!duckyFired)
	{
		duckyFired = true;

		GLfloat velocityX = -glm::sin(railgun->GetYaw() * RADIAN_CONVERSION) * LAUNCH_SPEED;
		GLfloat velocityY = glm::sin(railgun->GetPitch() * RADIAN_CONVERSION) * LAUNCH_SPEED;
		GLfloat velocityZ = -glm::cos(railgun->GetPitch() * RADIAN_CONVERSION) * LAUNCH_SPEED;

		ducky->SetVelocity(velocityX, velocityY, velocityZ);
	}
	else
	{
		// in manual mode, the ducky can be destroyed mid-flight
		if (mode == MANUAL && !paused)
		{
			duckyReFiredPosition = ducky->GetNewPosition();
			SetPreviousGameState(DUCKY_FIRED_MID_FLIGHT);
			duckiesLeft--;

			if (duckiesLeft == 0)
			{
				ResetGame();
			}
			else
			{
				ResetDucky();
			}
		}
	}
}

void Game::ToggleReplay()
{
	if (!duckyFired && replayType != NEW_GAME && mode == MANUAL)
	{
		replay = !replay;
	}

	else if (mode == AUTOMATED && replay){

		replay = FALSE;

	}
}

bool Game::GetReplay()
{
	return replay;
}

void Game::SetPreviousGameState(ReplayType type)
{
	prevScore = score;
	prevDuckiesLeft = duckiesLeft;
	prevPitch = railgun->GetPitch();
	prevYaw = railgun->GetYaw();
	replayType = type;
	prevRedHit = redHit;
	prevGreenHit = greenHit;
	prevBlueHit = blueHit;
}

void Game::Replay()
{
	if (!redHit && !blueHit && !greenHit && (duckiesLeft >= prevDuckiesLeft))
	{
		redBalloon->SetPosition(prevRedBalloonPosition.x, prevRedBalloonPosition.y, prevRedBalloonPosition.z);
		greenBalloon->SetPosition(prevGreenBalloonPosition.x, prevGreenBalloonPosition.y, prevGreenBalloonPosition.z);
		blueBalloon->SetPosition(prevBlueBalloonPosition.x, prevBlueBalloonPosition.y, prevBlueBalloonPosition.z);
	}

	redHit = prevRedHit;
	greenHit = prevGreenHit;
	blueHit = prevBlueHit;
	score = prevScore;
	duckiesLeft = prevDuckiesLeft;
	railgun->SetYaw(prevYaw);
	railgun->SetPitch(prevPitch);
	FireDucky();
}