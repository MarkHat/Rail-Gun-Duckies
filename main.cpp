#include <GL/freeglut.h>
#include "ducky.h"
#include "railgun.h"
#include "balloon.h"
#include "beauty.h"
#include "game.h"

const int FPS = 60;
const int PERIOD = 1000 / FPS;

const GLfloat TEXT_SIZE = GLfloat(0.12);
const glm::vec3 MODE_TEXT_POSITION = glm::vec3(10, 10, -1);
const glm::vec3 PAUSED_TEXT_POSITION = glm::vec3(10, 35, -1);
const glm::vec3 REPLAY_TEXT_POSITION = glm::vec3(10, 60, -1);

const char * RUBBER_DUCKY_BEAUTY_TEXT = "Rubber Ducky Beauty Mode";
const char * RAILGUN_BEAUTY_TEXT = "Railgun Beauty Mode";
const char * BALLOON_BEAUTY_TEXT = "Balloon Beauty Mode";
const char * MANUAL_TEXT = "Manual Mode";
const char * AUTOMATED_TEXT = "Automated Mode";
const char * PAUSED_TEXT = "(paused)";
const char * REPLAY_TEXT = "(replay)";

const int PERSPECTIVE_ANGLE = 45;
const int PERSPECTIVE_NEAR = 1;
const int PERSPECTIVE_FAR = 100;

const GLfloat MAT_SPECULAR[] = {1, 1, 1, 1};
const GLfloat MAT_SHININESS[] = {100};

const GLfloat LIGHT_0_POSITION[] = {1, 0.5, 0.75, 0};
const GLfloat LIGHT_1_POSITION[] = {-1, 0.5, 0.75, 0};
const GLfloat LIGHT_AMBIENT[] = {0, 0, 0, 0};
const GLfloat LIGHT_DIFFUSE[] = {GLfloat(0.4), GLfloat(0.4), GLfloat(0.4)};
const GLfloat LIGHT_SPECULAR[] = {GLfloat(0.7), GLfloat(0.7), GLfloat(0.7)};

int windowWidth = 800;
int windowHeight = 600;
double aspect = double(windowWidth) / double(windowHeight);

bool wireFrame;
bool debug;
bool paused;

enum GameModes
{
	RUBBER_DUCKY_BEAUTY,
	RAIL_GUN_BEAUTY,
	BALLOON_BEAUTY,
	MANUAL,
	AUTOMATED
};

enum GameModes gameMode = RUBBER_DUCKY_BEAUTY;

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
			Game::CycleGameMode();
			gameMode = MANUAL;

			// game mode uses only light
			glDisable(GL_LIGHT1);

			break;

		case MANUAL:
			Game::CycleGameMode();
			gameMode = AUTOMATED;

			break;

		case AUTOMATED:
			Game::CycleGameMode();
			gameMode = RUBBER_DUCKY_BEAUTY;

			// beauty mode uses two lights
			glEnable(GL_LIGHT1);

			break;
	}
}

void DisplayText(const char * text, glm::vec3 textPosition)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glDisable(GL_LIGHTING);

	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 1, 10);
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// draw the text
	glLoadIdentity();
	glColor3f(1, 1, 1);
	glTranslatef(textPosition.x, windowHeight - glutStrokeHeight(GLUT_STROKE_MONO_ROMAN) * TEXT_SIZE - textPosition.y, textPosition.z);
	glScaled(TEXT_SIZE, TEXT_SIZE, 1.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) text);

	glEnable(GL_LIGHTING);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

const char * AssignGameModeText()
{
	const char * TEXT;

	switch (gameMode)
	{
		case RUBBER_DUCKY_BEAUTY:
			TEXT = RUBBER_DUCKY_BEAUTY_TEXT;
			break;

		case RAIL_GUN_BEAUTY:
			TEXT = RAILGUN_BEAUTY_TEXT;
			break;

		case BALLOON_BEAUTY:
			TEXT = BALLOON_BEAUTY_TEXT;
			break;

		case MANUAL:
			TEXT = MANUAL_TEXT;
			break;

		case AUTOMATED:
			TEXT = AUTOMATED_TEXT;
			break;
	}

	return TEXT;
}

void Display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);

	// display all beauty/game mode content
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

	// display the game mode
	const char * TEXT = AssignGameModeText();
	DisplayText(TEXT, MODE_TEXT_POSITION);

	// display pause text
	if (paused)
	{
		DisplayText(PAUSED_TEXT, PAUSED_TEXT_POSITION);
	}

	// display replay text
	if (Game::GetReplay())
	{
		DisplayText(REPLAY_TEXT, REPLAY_TEXT_POSITION);
	}

	glutSwapBuffers();
}

void ReshapeFunc(int w, int h)
{
	// this prevents divide-by-zero errors
	if (h == 0)
	{
		return;
	}

	windowWidth = w;
	windowHeight = h;
	aspect = double(w) / double(h);
	
	Game::SetWindowDimensions(w, h);

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
			if (!Game::GetReplay())
			{
				Game::FireDucky();
			}
			break;
			break;

		case 'c':
		case 'C':
			Game::CycleCameraMode();
			break;

		case 'd':
		case 'D':
			Beauty::ToggleDebug();
			Game::ToggleDebug();
			break;

		case 'p':
		case 'P':
			paused = !paused;
			Game::TogglePause();
			break;

		case 'r':
		case 'R':
			Game::ToggleReplay();
			break;
	}
}

void MouseMotionFunc(int x, int y)
{
	// the railgun should only follow the mouse in manual, unpaused mode
	if (gameMode != AUTOMATED && !paused)
	{
		Game::MouseMotionFunc(x, y, windowWidth, windowHeight);
	}
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
	// beauty and game mode are updated once a frame
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

	glutTimerFunc(PERIOD, TimerFunc, value);
	glutPostRedisplay();
}

void CreateLights()
{
	glShadeModel(GL_SMOOTH);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/glMaterial.xml
	glMaterialfv(GL_FRONT, GL_SPECULAR, MAT_SPECULAR);
	glMaterialfv(GL_FRONT, GL_SHININESS, MAT_SHININESS);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/glLight.xml
	// light 0 information
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
	
	// light 1 information
	glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT_SPECULAR);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
}

int main(int argc, char * argv[])
{
	// initialize GL things
	glViewport(0, 0, windowWidth, windowHeight);
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Rail Gun Duckies");

	// initialize the perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(PERSPECTIVE_ANGLE, aspect, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);

	// reference: http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
	glEnable(GL_DEPTH_TEST);

	wireFrame = false;
	debug = false;
	paused = false;

	CreateLights();

	// set up callbacks
	glutDisplayFunc(Display); 
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutTimerFunc(PERIOD, TimerFunc, 0);

	// reference: http://www.opengl.org/resources/libraries/glut/spec3/node54.html
	glutSpecialFunc(SpecialFunc);

	// reference: http://www.lighthouse3d.com/tutorials/glut-tutorial/the-mouse/
	// note: active mouse motion and passive mouse motion are handled the same way
	glutMotionFunc(MouseMotionFunc);
	glutPassiveMotionFunc(MouseMotionFunc);

	glutMainLoop();

	Beauty::Destruct();
	Game::Destruct();

	return 0;
}