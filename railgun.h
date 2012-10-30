#include <GL/freeglut.h>
#include <glm/glm.hpp>

#pragma once

class RailGun
{
	public:
		RailGun();
		~RailGun();

		void Display();

		void SetYaw(int yaw);
		void SetPitch(int pitch);
		void UpdateYaw(int mouseX, int windowWidth);
		void UpdatePitch(int mouseY, int windowHeight);
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);

		float GetYaw();
		float GetPitch();
		glm::vec3 GetPosition();

	private:
		GLuint railgunDisplayList;

		float yaw;
		float pitch;
		glm::vec3 position;

		void CreateRailgunDisplayList();
};