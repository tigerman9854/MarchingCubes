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
glm::vec3 trans;
float scale = 0.5f;
MouseMode mouseMode = none;

const float scaleSens = 1.1f;
const float rotSens = 1.f;
const float panSens = 0.2f;
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
	if (mouseMode == rotate) {
		// Rotate the camera
		rotX += (y - mouseLastY) * rotSens;
		rotY += (x - mouseLastX) * rotSens;

		// Clamp x axis rotation between -90 and 90
		rotX = std::max(std::min(90.f, rotX), -90.f);
	} 
	else if (mouseMode == translate) {
		// Translate the camera
		trans.x += (x - mouseLastX) * panSens;
		trans.y -= (y - mouseLastY) * panSens;
	}

	mouseLastX = x;
	mouseLastY = y;

	glutPostRedisplay();
}

void display()
{
	// Get the width and height of the glut window for proper scaling and viewporting
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


	// Apply transformations in order 
	glScalef(scale, scale, scale);
	glTranslatef(trans.x, trans.y, trans.z);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);

	// Draw a point cloud
	glPointSize(5);
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

	// Draw all the cubes we've computed
	glLineWidth(1);
	glBegin(GL_LINES);
	if (scalarField) {
		for (auto it : scalarField->cubes) {
			auto drawVertex = [](Vertex* v) {
				if (v->val > 0) {
					glColor3f(1, 0, 0);
				}
				else {
					glColor3f(0, 1, 0);
				}
				glVertex3f(v->pos.x, v->pos.y, v->pos.z);
			};
			auto drawLine = [&](int a, int b) {
				drawVertex(it->vertices[a]);
				drawVertex(it->vertices[b]);
			};
				
			drawLine(0, 1);
			drawLine(1, 2);
			drawLine(2, 3);
			drawLine(3, 0);
			drawLine(0, 4);
			drawLine(4, 5);
			drawLine(5, 6);
			drawLine(6, 7);
			drawLine(7, 3);
			drawLine(6, 2);
			drawLine(5, 1);
		}
	}

	glVertex3f(-10.f, -10.f, -10.f);
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