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

		glm::vec3 GetOldPosition();
		glm::vec3 GetNewPosition();
		glm::vec3 GetVelocity();
		glm::vec3 GetBounds();

		void ToggleDebug();
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);
		void SetVelocity(GLfloat x, GLfloat y, GLfloat z);

	private:
		bool debug;

		GLuint duckyDisplayListHandle;
		GLuint boundingBoxDisplayListHandle;

		glm::vec3 oldPosition;
		glm::vec3 newPosition;
		glm::vec3 velocity;

		void CreateDuckyDisplayList();
		void CreateBoundingBoxDisplayList();
};
