#include "GL/glew.h"
#include "GL/freeglut.h"

#include "DataStructures.h"
#include "FileReader.h"

// Global dataset
ScalarField* scalarField;

enum MouseMode {
	none,
	translate,
	rotate,
};

// Variables for transformations
float rotX = 0;
float rotY = 0;
float scale = 0.5f;
MouseMode mouseMode = none;

const float scaleSens = 1.1f;
const float rotSens = 1.f;
int mouseLastX = 0;
int mouseLastY = 0;

// Forward declare functions
void init();
void keyboard(unsigned char key, int x, int y);
void motion(int x, int y);
void display();
void mouse(int button, int state, int x, int y);
void mousewheel(int wheel, int direction, int x, int y);

int main(int argc, char** argv) {
	// Initialize GLUT and create the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(960, 720);
	glutCreateWindow("Marching Cubes");

	// Initialize OpenGL
	init();

	// Link all the functions to glut
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutMouseWheelFunc(mousewheel);

	// Start the main loop, called many times per second
	glutMainLoop();
	return 0;
}

void init() 
{
	// Set the background color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Enable depth
	glEnable(GL_DEPTH_TEST);

	// Load a dataset
	scalarField = ReadFile("../Output/sphere.txt");
	//scalarField = ReadFile("../Output/saddle.txt");
}

void keyboard(unsigned char key, int x, int y)
{

}

void motion(int x, int y)
{
	rotX += (y - mouseLastY) * rotSens;
	rotY += (x - mouseLastX) * rotSens;

	rotX = std::max(std::min(90.f, rotX), -90.f);

	mouseLastX = x;
	mouseLastY = y;

	glutPostRedisplay();
}

void display()
{
	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)width / (double)height, 0.01, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 15., 0, 0, 0, 0, 1.0, 0);

	glScalef(scale, scale, scale);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);

	// Draw a point cloud
	glBegin(GL_POINTS);

	if (scalarField) {
		for (auto it : scalarField->vertices) {
			if (it->val > 0) {
				glColor3f(1, 0, 0);
			}
			else {
				glColor3f(0, 1, 0);
			}
			glVertex3f(it->pos.x, it->pos.y, it->pos.z);
		}
	}

	glEnd();

	// Swap buffers to show the newly rendered image
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON)
		{
			mouseMode = rotate;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			mouseMode = translate;
		}
		mouseLastX = x;
		mouseLastY = y;
	}
	else if (state == GLUT_UP) {
		mouseMode = none;
	}

	glutPostRedisplay();
}

void mousewheel(int wheel, int direction, int x, int y)
{
	if (direction == 1) {
		scale *= scaleSens;
		glutPostRedisplay();
	}
	else if (direction == -1) {
		scale /= scaleSens;
		glutPostRedisplay();
	}
}