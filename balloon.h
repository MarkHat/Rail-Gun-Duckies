#include <GL/freeglut.h>
#include <math.h>
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

	private:
		bool debug;

		vector<GLfloat> vaVertices;
		vector<GLuint> vaIndices;
		vector<GLfloat> vaNormals;

		vector<GLfloat> debugVertices;
		vector<GLuint> debugIndices;
		void DisplayFunc();

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