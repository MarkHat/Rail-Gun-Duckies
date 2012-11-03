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
		
		void Update(double elapsedTime, GLfloat speed, GLfloat gravity);
		void Display();

		glm::vec3 GetOldPosition();
		glm::vec3 GetNewPosition();
		glm::vec3 GetHeadPosition();
		glm::vec3 GetVelocity();
		glm::vec3 GetRotation();
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
		glm::vec3 headPosition;
		glm::vec3 velocity;
		glm::vec3 rotation;

		void CreateDuckyDisplayList();
		void CreateBoundingBoxDisplayList();
};
