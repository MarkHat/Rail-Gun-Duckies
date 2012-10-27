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
		
		float GetRadius();
		glm::vec3 GetPosition();
		void SetPosition(GLfloat x, GLfloat y, GLfloat z);

	private:
		GLuint vaDisplayListHandle;
		GLuint normalDisplayListHandle;
		GLuint boundingBoxDisplayListHandle;

		bool debug;

		vector<GLfloat> vaVertices;
		vector<GLuint> vaIndices;
		vector<GLfloat> vaNormals;

		vector<GLfloat> debugVertices;
		vector<GLuint> debugIndices;

		int slices;
		int stacks;
		float radius;

		glm::vec3 position;
		glm::vec3 bounds;

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

		void CreateBalloonDisplayList();
		void CreateNormalDisplayList();
		void CreateBoundingBoxDisplayList();
};