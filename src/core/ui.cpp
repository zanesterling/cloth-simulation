#include "ui.h"

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {
}

void UI::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	gluPerspective(FOV_Y, 1. * width / height, 0.1, 100);

	glVertexPointer(3, GL_FLOAT, 0, sim.triVerts);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());

	glPopMatrix();
	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}
