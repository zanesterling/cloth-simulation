#include "ui.h"

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {
}

void UI::update() {
	azimuth += vAzim;
	altitude += vAlt;
}

void UI::render() {
	GLfloat *tris = new GLfloat[9 * sim.getNumTris()];
	for (int i = 0; i < 9 * sim.getNumTris(); i++) {
		tris[i] = (GLfloat) sim.triVerts[i];
	}
	GLfloat *norms = new GLfloat[9 * sim.getNumTris()];
	for (int i = 0; i < 9 * sim.getNumTris(); i++) {
		norms[i] = (GLfloat) sim.norms[i];
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// perspective transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_Y, 1. * width / height, 0.1, 100);

	// model transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0, 0, -5);
	glRotatef(180. / M_PI * altitude, 1, 0, 0);
	glRotatef(180. / M_PI * azimuth,  0, 1, 0);

	glVertexPointer(3, GL_FLOAT, 0, tris);
	glNormalPointer(GL_FLOAT, 0, norms);

	glPolygonMode(GL_FRONT, fillMode ? GL_FILL : GL_LINE);

	// draw front of cloth
	glColor3f(0.3, 0.8, 0.9);
	glCullFace(GL_BACK);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());

	// invert normals and draw back
	for (int i = 0; i < 9 * sim.getNumTris(); i++) {
		norms[i] = -norms[i];
	}
	glColor3f(0.9, 0.4, 0.1);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 3 * sim.getNumTris());

	glPopMatrix();
	glutSwapBuffers();

	delete tris;
	delete norms;
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
