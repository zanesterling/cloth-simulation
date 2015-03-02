#include "ui.h"

void UI::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}
