#include "ui.h"

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
