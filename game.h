#include <GL/freeglut.h>
#include <time.h>
#include <stdio.h>

#include "ducky.h"
#include "railgun.h"
#include "balloon.h"

#pragma once

class Game 
{
	public:
		static void ToggleDebug();
		static void Update();
		static void Display();

		static void MouseMotionFunc(int x, int y, int windowWidth, int windowHeight);
		static void FireDucky();

		static void CycleGameMode();
		static void CycleCameraMode();
		static void SetWindowDimensions(int width, int height);

		static void ToggleReplay();
		static bool GetReplay();
		static void TogglePause();

		static void Destruct();

	private:
		static int score;
		static int duckiesLeft;
		static bool paused;

		static int windowWidth;
		static int windowHeight;

		static enum Mode mode;
		static enum CameraModes cameraMode;

		static Ducky * ducky;
		static RailGun * railgun;

		static Balloon * redBalloon;
		static Balloon * greenBalloon;
		static Balloon * blueBalloon;

		static bool redHit;
		static bool greenHit;
		static bool blueHit;

		static bool debug;
		static bool duckyFired;
		static double oldElapsedTime;
		static double newElapsedTime;

		static glm::vec3 cameraPosition;
		static glm::vec3 cameraLookAt;

		static double prevPitch;
		static double prevYaw;

		static int prevScore;
		static int prevDuckiesLeft;

		static enum ReplayType replayType;

		static glm::vec3 prevRedBalloonPosition;
		static glm::vec3 prevGreenBalloonPosition;
		static glm::vec3 prevBlueBalloonPosition;
		static glm::vec3 currRedBalloonPosition;
		static glm::vec3 currGreenBalloonPosition;
		static glm::vec3 currBlueBalloonPosition;
		static glm::vec3 duckyReFiredPosition;

		static bool replay;
		static double replayTimer;

		static bool prevRedHit;
		static bool prevGreenHit;
		static bool prevBlueHit;

		static void SetCamera();
		static char * AssignCameraModeText();

		static void DisplayOrthoText(const char * text, glm::vec3 position, GLfloat scale);
		static void DisplayBalloonText(Balloon * balloon, const char * balloonText);

		static void DisplayDucky();
		static void DisplayRailGun();
		static void DisplayBalloons();
		static void DisplayBalloonValues();
		static void DisplayGameInfo();
		static void DisplayScore();
		static void DisplayDuckiesLeft();
		static void DisplayCameraMode();

		static void RegenerateBalloons();
		static void ResetGame();
		static void ResetDucky();
		static void ResetBalloons();

		static bool CheckDuckyBalloonCollision(glm::vec3 duckyTempPosition, Balloon * balloon);
		static void HandleCollisions();

		static GLfloat EstimateTargetPitch(float xDifference, float yDifference, float zDifference, float targetY);
		static void AutomateRailgun();

		static void SetPreviousGameState(ReplayType replayType);
		static void Replay();
};