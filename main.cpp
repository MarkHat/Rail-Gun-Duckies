#include <GL/freeglut.h>
#include "ducky.h"
#include "railgun.h"
#include "balloon.h"
#include "beauty.h"
#include "game.h"

int windowWidth = 800;
int windowHeight = 600;
double aspect = double(windowWidth) / double(windowHeight);

const int fps = 60;
const int period = 1000 / fps;
bool wireFrame;
bool debug;
bool paused;

enum CameraModes
{
	FIXED_POSITION,
	REORIENT,
	FIRST_PERSON
};

enum GameModes
{
	RUBBER_DUCKY_BEAUTY,
	RAIL_GUN_BEAUTY,
	BALLOON_BEAUTY,
	MANUAL,
	AUTOMATED
};

enum CameraModes cameraMode = FIXED_POSITION;
enum GameModes gameMode = RUBBER_DUCKY_BEAUTY;

void CycleCameraMode()
{
	switch(cameraMode)
	{
		case FIXED_POSITION:
			cameraMode = REORIENT;
			break;

		case REORIENT:
			cameraMode = FIRST_PERSON;
			break;

		case FIRST_PERSON:
			cameraMode = FIXED_POSITION;
			break;
	}
}

void CycleGameMode()
{
	switch(gameMode)
	{
		case RUBBER_DUCKY_BEAUTY:
			Beauty::CycleMode();
			gameMode = RAIL_GUN_BEAUTY;

			break;

		case RAIL_GUN_BEAUTY:
			Beauty::CycleMode();
			gameMode = BALLOON_BEAUTY;

			break;

		case BALLOON_BEAUTY:
			Beauty::CycleMode();
			Game::CycleMode();
			gameMode = MANUAL;

			break;

		case MANUAL:
			Game::CycleMode();
			gameMode = AUTOMATED;
			break;

		case AUTOMATED:
			Beauty::CycleMode();
			Game::CycleMode();
			gameMode = RUBBER_DUCKY_BEAUTY;

			break;
	}
}

void DisplayText(char * text, int fromLeft, int fromTop)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 1, 10);
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glColor3f(1, 1, 1);
	glTranslatef(fromLeft, windowHeight - glutStrokeHeight(GLUT_STROKE_MONO_ROMAN) * 0.15 - fromTop, -5.5);
	glScaled(0.15, 0.15, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) text);

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

char * AssignGameModeText()
{
	char * text;

	switch (gameMode)
	{
		case RUBBER_DUCKY_BEAUTY:
			text = "Rubber Ducky Beauty Mode";
			break;

		case RAIL_GUN_BEAUTY:
			text = "Rail Gun Beauty Mode";
			break;

		case BALLOON_BEAUTY:
			text = "Balloon Beauty Mode";
			break;

		case MANUAL:
			text = "Manual Mode";
			break;

		case AUTOMATED:
			text = "Automated Mode";
			break;
	}

	return text;
}

void DisplayText(char * text)
{
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glColor3f(1, 1, 1);
	glTranslatef(10, windowHeight - glutStrokeHeight(GLUT_STROKE_MONO_ROMAN) * 0.15 - 10, -5.5);
	glScaled(0.15, 0.15, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) text);

	glPopMatrix();
}

void Display()
{
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);

	switch(gameMode)
	{
		case RUBBER_DUCKY_BEAUTY:
		case RAIL_GUN_BEAUTY:
		case BALLOON_BEAUTY:
			Beauty::Display();
			break;

		case MANUAL:
		case AUTOMATED:
			Game::Display();
			break;
	}

	char * text = AssignGameModeText();
	DisplayText(text, 10, 10);

	if (paused)
	{
		DisplayText("(paused)", 10, 40);
	}

	glutSwapBuffers();
}

void ReshapeFunc(int w, int h)
{
	// this prevents divide by zero errors
	if (h == 0)
	{
		return;
	}

	windowWidth = w;
	windowHeight = h;
	aspect = double(w) / double(h);
	
	glutPostRedisplay();
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	switch (c) {
		case 'x':
		case 'X':

		// escape
		case 27:
			glutLeaveMainLoop();
			return;

		case 'w':
		case 'W':
			wireFrame = !wireFrame;
			break;

		// space
		case 32:
			Game::FireDucky();
			break;

		case 'c':
		case 'C':
			CycleCameraMode();
			break;

		case 'd':
		case 'D':
			Beauty::ToggleDebug();
			break;

		case 'p':
		case 'P':
			paused = !paused;
			break;
	}
}

void MouseMotionFunc(int x, int y)
{
	Game::MouseMotionFunc(x, y, windowWidth, windowHeight);
}

void SpecialFunc(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		CycleGameMode();
	}
}

void TimerFunc(int value)
{
	switch (gameMode)
	{
		case RUBBER_DUCKY_BEAUTY:
		case RAIL_GUN_BEAUTY:
		case BALLOON_BEAUTY:
			Beauty::Update(paused);
			break;

		case MANUAL:
		case AUTOMATED:
			Game::Update();
			break;
	}

	glutTimerFunc(period, TimerFunc, value);
	glutPostRedisplay();
}

void CreateLight()
{
	GLfloat matSpecular[] = {1, 1, 1, 1};
	GLfloat matShininess[] = {100};
	GLfloat lightPosition[] = {1, 1, 1, 0};

	glShadeModel(GL_SMOOTH);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/glMaterial.xml
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/glLight.xml
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Rail Gun Duckies");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
	gluPerspective(45, aspect, 1, 30);
	glEnable(GL_DEPTH_TEST);

	wireFrame = false;
	debug = false;
	paused = false;

	CreateLight();

	glutDisplayFunc(Display); 
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutTimerFunc(period, TimerFunc, 0);

	// reference: http://www.opengl.org/resources/libraries/glut/spec3/node54.html
	glutSpecialFunc(SpecialFunc);

	// reference: http://www.lighthouse3d.com/tutorials/glut-tutorial/the-mouse/
	// note: active mouse motion and passive mouse motion are handled the same way
	glutMotionFunc(MouseMotionFunc);
	glutPassiveMotionFunc(MouseMotionFunc);

	glutMainLoop();

	return 0;
}