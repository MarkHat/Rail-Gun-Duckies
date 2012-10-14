#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>

#pragma once

using namespace std;

class Ducky
{
	public:
		Ducky();
		~Ducky();

		void Display();
		glm::vec3 GetPosition();
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);
		void SetVelocity(GLfloat x, GLfloat y, GLfloat z);
		void Update(double elapsedTime);

	private:
		GLuint displayListHandle;

		glm::vec3 position;
		glm::vec3 velocity;
};
