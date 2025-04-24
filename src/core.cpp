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

	glutDisplayFunc([]() { ui.render(); });
	glutIdleFunc(idle);
	glutReshapeFunc([](int w, int h) { ui.resize(w, h); });
	glutKeyboardFunc(keyboard);
	glutMouseFunc([](int button, int state, int x, int y) { ui.mouse(button, state, x, y); });
	glutMotionFunc([](int x, int y) { ui.mouseMotion(x, y); });

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

void idle() {
	sim.update();
	ui.update();

	// mark dirty frame
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		// View control.
		case 'w':
			ui.vAlt = 0.01;
			break;
		case 's':
			ui.vAlt = -0.01;
			break;
		case 'd':
			ui.vAzim = -0.01;
			break;
		case 'a':
			ui.vAzim = 0.01;
			break;
		case 'e':
			ui.vAzim = 0;
			ui.vAlt = 0;
			break;

		// Render settings.
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
		case 't':
			lighting_enabled = !lighting_enabled;
			if (lighting_enabled) {
				glEnable(GL_LIGHTING);
			} else {
				glDisable(GL_LIGHTING);
			}
			break;

		// Simulation settings.
		case 'p':
			sim.running = !sim.running;
			break;
		case 'r':
			sim.reset();
			break;
		case 'i':
			sim.bannerStyle = !sim.bannerStyle;
			break;
		case 'o':
			sim.cuffing = !sim.cuffing;
			break;
		case '[':
			sim.prevScene();
			break;
		case ']':
			sim.nextScene();
			break;

		case 'q':
			quit();
			break;
	}
}

void quit() {
	printf("quitting; cheers!\n");
	exit(0);
}
