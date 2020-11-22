#include "GL/glew.h"
#include "GL/freeglut.h"



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
}

void keyboard(unsigned char key, int x, int y)
{

}

void motion(int x, int y)
{

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

	// Draw a single point
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glEnd();

	// Swap buffers to show the newly rendered image
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{

}

void mousewheel(int wheel, int direction, int x, int y)
{

}