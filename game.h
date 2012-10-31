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
		static void Update(bool paused);
		static void Display();
		static void ToggleDebug();
		static void CycleMode();
		static void MouseMotionFunc(int x, int y, int windowWidth, int windowHeight);
		static void FireDucky();
		static void CycleCameraMode();
		static void SetWindowDimensions(int width, int height);
		static void Replay();

	private:
		static int score;
		static int duckiesLeft;
		static bool paused;
		static double pausedTime;
		static double difference;

		static int windowWidth;
		static int windowHeight;

		static glm::vec3 duckyInitialPosition;
		static glm::vec3 gunInitialPosition;
		static GLfloat launchSpeed;
		static GLfloat radianConversion;
		static GLfloat gravity;

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

		static void SetCamera();
		static char * ConvertToString(int value);

		static void DisplayXYZ();
		static void DisplayDucky();
		static void DisplayRailGun();
		static void DisplayBalloons();
		static void DisplayText(char * text);
		static void DisplayBalloonText();
		static void DisplayGameInfo();

		static void GenerateBalloons();
		static void ResetGame();
		static void ResetDucky();
		static void ResetBalloons();

		static bool CheckDuckyBalloonCollision(glm::vec3 duckyTempPosition, Balloon * balloon);
		static void HandleCollisions();

		static void AutomateRailgun();
};