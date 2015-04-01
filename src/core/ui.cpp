#include "ui.h"

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {
}

void UI::render() {
	GLfloat *tris = new GLfloat[9 * sim.getNumTris()];
	for (int i = 0; i < 9 * sim.getNumTris(); i++) {
		tris[i] = (GLfloat) sim.triVerts[i];
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluPerspective(FOV_Y, 1. * width / height, 0.1, 100);

	glVertexPointer(3, GL_FLOAT, 0, tris);

	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());
	glColor3f(0, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());

	glPopMatrix();
	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}

void UI::mouse(int button, int state, int x, int y) {
	mouseLoc[0] = x;
	mouseLoc[1] = y;

	// TODO deal with perspective transform
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			selectedVert = -1;
		} else {
			// for each point in the cloth mesh
			Cloth &cloth = sim.cloth;
			for (int i = 0; i < cloth.xRes * cloth.yRes; i++) {
				// if the point is close to the click, select it
				auto point = Vector2d(cloth.getWorldPoint(i));
				if ((point - mouseLoc).norm() < CLICK_RADIUS) {
					selectedVert = i;
				}
			}
		}
	}
}

void UI::mouseMotion(int x, int y) {
	if (selectedVert == -1) return;

	mouseLoc[0] = x;
	mouseLoc[1] = y;

	auto point = sim.cloth.getWorldPoint(selectedVert);
	point[0] = x;
	point[1] = y;
}
