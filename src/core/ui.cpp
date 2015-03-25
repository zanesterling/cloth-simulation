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
