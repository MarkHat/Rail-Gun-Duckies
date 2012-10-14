#include <GL/freeglut.h>
#include "ducky.h"
#include "railgun.h"
#include "balloon.h"

#pragma once

class Game 
{
	public:
		static void Update();
		static void Display();
		static void ToggleDebug();
		static void CycleMode();
		static void MouseMotionFunc(int x, int y, int windowWidth, int windowHeight);
		static void FireDucky();

	private:
		static enum Mode mode;

		static Ducky * ducky;
		static RailGun * railgun;
		static Balloon * balloon;

		static bool debug;
		static bool duckyFired;
		static double oldElapsedTime;
		static double newElapsedTime;

		static void DisplayXYZ();
		static void DisplayDucky();

		static void ResetGame();
		static void ResetDucky();
};