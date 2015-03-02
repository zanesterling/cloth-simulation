#include "ui.h"
#include <stdio.h>

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {

	for (int i = 0; i < sim.cloth.h; i++) {
		for (int j = 0; j < sim.cloth.w; j++) {
			GLfloat *point = sim.cloth.uvPoints + 2*(i * sim.cloth.w + j);
			printf("%f,%f\t", point[0], point[1]);
		}
		printf("\n");
	}
}

void UI::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glVertexPointer(3, GL_ARRAY_BUFFER, 0, sim.triVerts);
	glDrawArrays(GL_TRIANGLES, 0, sim.getNumTris() * 3);

	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}
