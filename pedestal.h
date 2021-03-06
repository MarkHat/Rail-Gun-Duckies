#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>

#pragma once

using namespace std;

class Pedestal
{
	public:
		Pedestal();
		~Pedestal();

		void ToggleDebug();
		void Display();

	private:
		GLuint pedestalDisplayList;
		GLuint debugDisplayList;

		bool debug;

		vector<GLfloat> vaVertices;
		vector<GLuint> vaIndices;
		vector<GLfloat> vaNormals;

		vector<GLfloat> debugVertices;
		vector<GLuint> debugIndices;

		void ComputeVaVertices();
		void GenerateVaIndices();
		void ComputeVaNormals();

		void ComputeDebugVertices();
		void GenerateDebugIndices();

		void CreatePedestalDisplayList();
		void CreateDebugDisplayList();
};