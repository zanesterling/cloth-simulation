#include "core.h"

#define global_variable static;

global_variable bool lighting_enabled = true;

int main(int argc, char **argv) {
	initGlut(argc, argv);
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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(0., 0., 0., 1.);
}

void initLights() {
	GLfloat lightPos0[3] = {1, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	GLfloat lightAmb0[4] = {0.6, 0.6, 0.6, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
	GLfloat lightDif0[4] = {0.6, 0.6, 0.6, 1};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif0);
}

void display() {
	ui.render();
}

void idle() {
	sim.update();
	ui.update();

	// mark dirty frame
	glutPostRedisplay();
}

void resize(int w, int h) {
	ui.resize(w, h);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
			ui.vAlt = 0.02;
			break;
		case 's':
			ui.vAlt = -0.02;
			break;
		case 'd':
			ui.vAzim = -0.02;
			break;
		case 'a':
			ui.vAzim = 0.02;
			break;
		case 'e':
			ui.vAzim = 0;
			ui.vAlt = 0;
			break;
		case 'p':
			sim.running = !sim.running;
			break;
		case 'r':
			sim.reset();
			break;
		case 'q':
			quit();
			break;
		case 't':
			lighting_enabled = !lighting_enabled;
			if (lighting_enabled) {
				glEnable(GL_LIGHTING);
			} else {
				glDisable(GL_LIGHTING);
			}
			break;
		case 'y':
			if (ui.frontColor == WHITE) {
				ui.frontColor = DEFAULT_FRONT_COLOR;
				ui.backColor  = DEFAULT_BACK_COLOR;
			} else {
				ui.frontColor = WHITE;
				ui.backColor  = WHITE;
			}
			break;
		case 'u':
			ui.fillMode = !ui.fillMode;
			break;
		case 'i':
			sim.bannerStyle = !sim.bannerStyle;
			break;
		case 'o':
			sim.cuffing = !sim.cuffing;
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
