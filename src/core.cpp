#include "core.h"

int main(int argc, char **argv) {
	initGlut(argc, argv);
	glewInit();
	initLights();

	glutMainLoop();
}

void initGlut(int argc, char **argv) {
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(ui.width, ui.height);
	glutCreateWindow("Dimensions");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glShadeModel(GL_FLAT);

	glClearColor(0., 0., 0., 1.);
}

void initLights() {
	GLfloat lightPos0[3] = {7, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	GLfloat lightAmb0[8] = {0.8, 0.8, 0.8, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
}

void display() {
	ui.render();
}

void idle() {
	sim.update();

	// mark dirty frame
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {}
