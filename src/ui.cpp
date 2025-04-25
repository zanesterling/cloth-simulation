#include "ui.h"

UI::UI(int canvasWidth, int canvasHeight, Simulation &sim)
	: width(canvasWidth), height(canvasHeight), sim(sim) {
}

void UI::update() {
	azimuth += vAzim;
	altitude += vAlt;
}

static GLfloat *triangles = NULL;
static GLfloat *normals = NULL;
static GLdouble *colors = NULL;
static double *forces = NULL;
static int numTriangles = 0;
static int numPoints = 0;

void UI::render() {
	int simNumTris = sim.getNumTris();
	if (triangles == NULL || normals == NULL || numTriangles != simNumTris) {
		if (triangles != NULL) delete triangles;
		if (normals != NULL) delete normals;
		if (colors != NULL) delete colors;
		if (forces != NULL) delete forces;
		numTriangles = simNumTris;
		numPoints = 3 * numTriangles;
		triangles = new GLfloat[3 * numPoints];
		normals   = new GLfloat[3 * numPoints];
		colors    = new GLdouble[3 * numPoints];
		forces    = new double[3 * numPoints];
	}
	for (int i = 0; i < 9 * simNumTris; i++) {
		triangles[i] = (GLfloat) sim.triVerts[i];
		normals[i] = (GLfloat) sim.triNorms[i];
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

	glVertexPointer(3, GL_FLOAT, 0, triangles);
	glNormalPointer(GL_FLOAT, 0, normals);

	if (COLORS_ENABLED) {
		double largestScaleColor = 0.0;
		double largestShearColor = 0.0;
		double largestBendColor = 0.0;
		copyForcesFromMatrix(forces, sim.scaleForces, sim.cloth.xRes, sim.cloth.yRes);
		for (int pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			double *force = forces + pointIndex*3;
			double fx = force[0];
			double fy = force[1];
			double fz = force[2];
			double magnitude = fx*fx + fy*fy + fz*fz;
			double color = magnitude * 10 * 1000 * 1000 * 1000;
			colors[pointIndex*3 + 0] = 0.1 + color; // set red
			if (color > largestScaleColor) largestScaleColor = color;
		}
		copyForcesFromMatrix(forces, sim.shearForces, sim.cloth.xRes, sim.cloth.yRes);
		for (int pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			double *force = forces + pointIndex*3;
			double fx = force[0];
			double fy = force[1];
			double fz = force[2];
			double magnitude = fx*fx + fy*fy + fz*fz;
			double color = magnitude * 10 * 1000 * 1000 * 1000;
			colors[pointIndex*3 + 1] = 0.1 + color; // set green
			if (color > largestShearColor) largestShearColor = color;
		}
		copyForcesFromMatrix(forces, sim.bendForces, sim.cloth.xRes, sim.cloth.yRes);
		for (int pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			double *force = forces + pointIndex*3;
			double fx = force[0];
			double fy = force[1];
			double fz = force[2];
			double magnitude = fx*fx + fy*fy + fz*fz;
			double color = magnitude * 10 * 1000 * 1000 * 1000;
			colors[pointIndex*3 + 2] = 0.1 + color; // set blue
			if (color > largestBendColor) largestBendColor = color;
		}

		glColorPointer(3, GL_DOUBLE, 0, colors);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	glPolygonMode(GL_FRONT_AND_BACK, fillMode ? GL_FILL : GL_LINE);

	// draw front of cloth
	// glColor3f(frontColor.red, frontColor.green, frontColor.blue);
	glCullFace(GL_BACK);
	glDrawArrays(GL_TRIANGLES, 0, numPoints);

	// invert normals and draw back
	for (int i = 0; i < 9 * numTriangles; i++) {
		normals[i] = -normals[i];
	}
	// glColor3f(backColor.red, backColor.green, backColor.blue);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, numPoints);

	glPopMatrix();
	glutSwapBuffers();
}

void UI::resize(int w, int h) {
	width  = w;
	height = h;
}

void UI::mouse(int button, int state, int x, int y) {
	this->mouseX = x;
	this->mouseY = y;
	this->button = button;
}

void UI::mouseMotion(int x, int y) {
	float dx = x - this->mouseX;

	if (button == GLUT_LEFT_BUTTON) {
		sim.changeScaleX(dx / width);
	} else if (button == GLUT_RIGHT_BUTTON) {
		sim.changeCuff(dx / width);
	}

	this->mouseX = x;
	this->mouseY = y;
}
