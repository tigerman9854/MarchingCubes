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

enum DisplayMode {
	points,
	edges,
	cubes,
	marching,
};

// Variables for transformations
float rotX = 0;
float rotY = 0;
glm::vec3 trans;
float scale = 0.5f;
MouseMode mouseMode = none;
DisplayMode displayMode = points;

const float scaleSens = 1.1f;
const float rotSens = 1.f;
const float panSens = 0.15f;
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
	//scalarField = ReadFile("../Output/sphere.txt");
	scalarField = ReadFile("../Output/saddle.txt");
	//scalarField = ReadFile("../Output/cone.txt");
	//scalarField = ReadFile("../Output/parabola.txt");
	//scalarField = ReadFile("../Output/chair.txt");
	//scalarField = ReadFile("../Output/conetear.txt");
	//scalarField = ReadFile("../Output/curve.txt");
	//scalarField = ReadFile("../Output/curve2.txt");
}

void keyboard(unsigned char key, int x, int y)
{
	// Change mode based on keypress
	switch (key) {
	case '1':
		displayMode = points;
		break;
	case '2':
		displayMode = edges;
		break;
	case '3':
		displayMode = cubes;
		break;
	case '4':
		displayMode = marching;
		break;
	}

	glutPostRedisplay();
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

	if (scalarField) {
		if (displayMode == points)
		{
			// Draw a point cloud
			glPointSize(2);
			glBegin(GL_POINTS);
			for (auto it : scalarField->vertices) {
				if (it->val < 0) {
					glColor3f(1, 0, 0);
				}
				else {
					glColor3f(0, 1, 0);
				}
				glVertex3f(it->pos.x, it->pos.y, it->pos.z);
			}
			glEnd();
		}
		else if (displayMode == edges) {
			// Draw the edges of the cubes
			glLineWidth(1);
			glBegin(GL_LINES);

			auto drawVertex = [](Vertex* v) {
				if (v->val < 0) {
					glColor3f(1, 0, 0);
				}
				else {
					glColor3f(0, 1, 0);
				}
				glVertex3f(v->pos.x, v->pos.y, v->pos.z);
			};

			for (auto it : scalarField->cubes) {

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
			
			glEnd();
		}
		else if (displayMode == cubes) {
			glBegin(GL_QUADS);

			auto drawVertex = [](Vertex* v) {
				glVertex3f(v->pos.x, v->pos.y, v->pos.z);
			};

			// Helper which determines if a vertex exists on either side of the threshold
			auto shouldDraw = [](Cube* c) {
				bool above = false;
				bool below = false;
				for (Vertex* v : c->vertices) {
					if (v->val > 0) {
						above = true;
					}
					else {
						below = true;
					}
				}
				return above && below;
			};


			for (auto it : scalarField->cubes) {
				// Only draw this cube if it contains at least 1 vertex on either side of the threshold
				if (!shouldDraw(it)) {
					continue;
				}

				// Bottom and top
				glColor3f(0.6f, 0, 0);
				drawVertex(it->vertices[0]);
				drawVertex(it->vertices[1]);
				drawVertex(it->vertices[2]);
				drawVertex(it->vertices[3]);

				drawVertex(it->vertices[4]);
				drawVertex(it->vertices[5]);
				drawVertex(it->vertices[6]);
				drawVertex(it->vertices[7]);

				// Front and back
				glColor3f(0.8f, 0, 0);
				drawVertex(it->vertices[0]);
				drawVertex(it->vertices[1]);
				drawVertex(it->vertices[5]);
				drawVertex(it->vertices[4]);

				drawVertex(it->vertices[2]);
				drawVertex(it->vertices[3]);
				drawVertex(it->vertices[7]);
				drawVertex(it->vertices[6]);

				// Right and left
				glColor3f(1.f, 0, 0);
				drawVertex(it->vertices[0]);
				drawVertex(it->vertices[4]);
				drawVertex(it->vertices[7]);
				drawVertex(it->vertices[3]);

				drawVertex(it->vertices[1]);
				drawVertex(it->vertices[2]);
				drawVertex(it->vertices[6]);
				drawVertex(it->vertices[5]);
			}

			glEnd();
		}
		else if (displayMode == marching) {
			// Set up 2 lights directly opposite eachother
			glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHT1);

			float globalAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.f };
			float pos[4] = { 50.f, 0.f, 15.f, 1.f };
			float pos2[4] = { -50.f, 0.f, -15.f, 1.f };
			float diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.f };

			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
			glLightfv(GL_LIGHT0, GL_POSITION, pos);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT1, GL_POSITION, pos2);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

			// Set up red material
			float ambientColor[4] = { 1.f, 0.f, 0.f, 1.f };
			float diffuseColor[4] = { 1.f, 0.f, 0.f, 1.f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);

			// Draw the triangles computed by marching squares
			glBegin(GL_TRIANGLES);
			for (auto it : scalarField->cubes) {
				for (auto tri : it->marchingCubesResults) {
					glNormal3f(tri->normal.x, tri->normal.y, tri->normal.z);
					glVertex3f(tri->vertices[0]->pos.x, tri->vertices[0]->pos.y, tri->vertices[0]->pos.z);
					glVertex3f(tri->vertices[1]->pos.x, tri->vertices[1]->pos.y, tri->vertices[1]->pos.z);
					glVertex3f(tri->vertices[2]->pos.x, tri->vertices[2]->pos.y, tri->vertices[2]->pos.z);
				}
			}

			glEnd();

			// Disable all the things we enabled
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT0);
			glDisable(GL_NORMALIZE);
			glDisable(GL_LIGHTING);

		}
	}

	// Swap buffers to show the newly rendered image
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	// Left button to rotate, right button to pan
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
	// Zoom with mouse wheel
	if (direction == 1) {
		scale *= scaleSens;
		glutPostRedisplay();
	}
	else if (direction == -1) {
		scale /= scaleSens;
		glutPostRedisplay();
	}
}