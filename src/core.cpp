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
	glutCreateWindow("Cloth simulation");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	glEnableClientState(GL_VERTEX_ARRAY);

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

void resize(int w, int h) {
	ui.resize(w, h);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
			quit();
			break;
	}
}

void mouse(int button, int state, int x, int y) {
	ui.mouse(button, state, x, y);
}

void mouseMotion(int x, int y) {
	ui.mouseMotion(x, y);
}

void quit() {
	printf("quitting; cheers!\n");
	exit(0);
}
