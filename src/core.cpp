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
	glutReshapeFunc([](int w, int h) { ui.resize(w, h); });
	glutKeyboardFunc(keyboard);
	glutMouseFunc([](int button, int state, int x, int y) { ui.mouse(button, state, x, y); });
	glutMotionFunc([](int x, int y) { ui.mouseMotion(x, y); });
	frame(0);

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
	GLfloat lightPos0[4] = {1, 1, 0, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	GLfloat lightAmb0[4] = {0.0, 0.0, 0.0, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
	GLfloat lightDif0[4] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif0);
}

constexpr long FPS = 60;
constexpr long N_FRAMES_TO_BLUR = 60;
static long frameDurMicrosSum = 0;
void frame(int frameCount) {
	struct timespec startTime, endTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);

	sim.update();
	ui.update();

	clock_gettime(CLOCK_MONOTONIC, &endTime);
	long frameDurNanos =
		(endTime.tv_sec - startTime.tv_sec) * 1000000L +
		(endTime.tv_nsec - startTime.tv_nsec);
	frameDurMicrosSum += frameDurNanos / 1000L;
	if (frameCount % N_FRAMES_TO_BLUR == 0) {
		long micros = frameDurMicrosSum / N_FRAMES_TO_BLUR;
		printf("frame: %ld.%ldms\n", micros / 1000, micros % 1000);
		frameDurMicrosSum = 0;
	}

	// mark dirty frame
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, frame, frameCount + 1);
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
