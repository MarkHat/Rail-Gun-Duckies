#include <GL/freeglut.h>
#include <glm/glm.hpp>

#pragma once

class RailGun
{
	public:
		RailGun();
		~RailGun();

		void Display();

		void SetYaw(GLfloat yaw);
		void SetPitch(GLfloat pitch);
		void SetTargetYaw(GLfloat targetYaw);
		void SetTargetPitch(GLfloat targetPitch);
		void UpdateYaw(int mouseX, int windowWidth);
		void UpdatePitch(int mouseY, int windowHeight);
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);

		void SetAnimating();
		void MoveTowardsTarget();

		float GetYaw();
		float GetPitch();
		glm::vec3 GetPosition();
		bool IsAnimating();

	private:
		GLuint railgunDisplayList;

		GLfloat yaw;
		GLfloat pitch;
		GLfloat targetYaw;
		GLfloat targetPitch;
		glm::vec3 position;

		float yawIncrement;
		float pitchIncrement;
		bool animating;
		int animationFrame;

		void CreateRailgunDisplayList();
};