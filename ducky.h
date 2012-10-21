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
		
		void Update(double elapsedTime, GLfloat gravity);
		void Display();

		glm::vec3 GetPosition();
		glm::vec3 GetVelocity();
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);
		void SetVelocity(GLfloat x, GLfloat y, GLfloat z);

	private:
		GLuint duckyDisplayList;

		glm::vec3 position;
		glm::vec3 velocity;
};
