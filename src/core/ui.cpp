#include "ui.h"
#include <stdio.h>

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {
}

void UI::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glVertexPointer(3, GL_FLOAT, 0, sim.triVerts);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());

	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}
