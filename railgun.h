#include <GL/freeglut.h>

#pragma once

class RailGun
{
	public:
		RailGun();
		~RailGun();

		void Display();
		void UpdateYaw(int mouseX, int windowWidth);
		void UpdatePitch(int mouseY, int windowHeight);
		float GetYaw();
		float GetPitch();

	private:
		float yaw;
		float pitch;
};