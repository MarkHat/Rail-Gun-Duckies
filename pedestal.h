#include <GL/freeglut.h>
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
		bool debug;

		vector<GLfloat> vaVertices;
		vector<GLuint> vaIndices;
		vector<GLfloat> vaNormals;

		vector<GLfloat> debugVertices;
		vector<GLuint> debugIndices;

		int slices;
		int stacks;
		float radius;
		float height;

		int numVaVertices;
		int numVaIndices;
		int numDebugVertices;
		int numDebugIndices;

		void ComputeVaVertices();
		void GenerateVaIndices();
		void ComputeVaNormals();

		void ComputeDebugVertices();
		void GenerateDebugIndices();
};