#include <GL/freeglut.h>
#include "ducky.h"
#include "railgun.h"
#include "balloon.h"
#include "pedestal.h"

#pragma once

class Beauty
{
	public:
		static void Update(bool paused);
		static void Display();

		static void ToggleDebug();
		static void CycleMode();

		static void Destruct();

	private:
		static enum Mode mode;

		static Ducky * ducky;
		static RailGun * railgun;
		static Balloon * balloon;
		static Pedestal * pedestal;

		static bool debug;
		static double oldElapsedTime;
		static double newElapsedTime;
		static double pausedTime;
		static double realElapsedTime;

		static void DisplayXYZ();
};