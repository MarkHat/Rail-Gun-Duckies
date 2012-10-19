#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>

#pragma once

using namespace std;

class Balloon
{
	public:
		Balloon();
		~Balloon();

		void ToggleDebug();
		void Display();

		void SetPosition(GLfloat x, GLfloat y, GLfloat z);
		glm::vec3 GetPosition();

	private:
		bool debug;

		vector<GLfloat> vaVertices;
		vector<GLuint> vaIndices;
		vector<GLfloat> vaNormals;

		vector<GLfloat> debugVertices;
		vector<GLuint> debugIndices;
		void DisplayFunc();

		glm::vec3 position;

		int slices;
		int stacks;
		float radius;

		int numVaVertices;
		int numVaIndices;
		int numDebugVertices;
		int numDebugIndices;

		void ComputeVaVertices();
		void GenerateVaIndices();
		void ComputeVaNormals();

		void ComputeDebugVertices();
		void GenerateDebugIndices();
		int numVertices;
		int numIndices;

		void ComputeVertices();
		void GenerateIndices();
		void ComputeNormals();
};