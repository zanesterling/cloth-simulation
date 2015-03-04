#include "cloth.h"

Cloth::Cloth(int xRes, int yRes)
	: xRes(xRes), yRes(yRes), w(1), h(1) {

	initUvPoints();
	initWorldPoints();
}

Cloth::Cloth(int xRes, int yRes, GLfloat w, GLfloat h)
	: xRes(xRes), yRes(yRes), w(w), h(h) {

	initUvPoints();
	initWorldPoints();
}

// initialize uvPoints with uniform grid
void Cloth::initUvPoints() {
	uvPoints = new GLfloat[2 * xRes * yRes];
	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			uvPoints[2 * (i*xRes + j)]     = j;
			uvPoints[2 * (i*xRes + j) + 1] = i;
		}
	}
}

// initializte cloth world-state
void Cloth::initWorldPoints() {
	// init world positions
	worldPoints = new GLfloat[3 * xRes * yRes];
	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			GLfloat *point = getUvPoint(j, i);
			setWorldPoint(j, i, point[0], point[1], 0);
		}
	}

	// init velocities
	worldVels = new GLfloat[3 * xRes * yRes];
	for (int i = 0; i < 3 * yRes*xRes; i++) {
		worldVels[i] = 0;
	}
}

// NOTE: doesn't check bounds!
GLfloat *Cloth::getUvPoint(int x, int y) {
	return uvPoints + 2 * (y*xRes + x);
}

// NOTE: doesn't check bounds!
GLfloat *Cloth::getWorldPoint(int x, int y) {
	return worldPoints + 3 * (y*xRes + x);
}

void Cloth::setWorldPoint(int xCor, int yCor, GLfloat xPos, GLfloat yPos,
	GLfloat zPos) {
	GLfloat *point = getWorldPoint(xCor, yCor);
	point[0] = xPos;
	point[1] = yPos;
	point[2] = zPos;
}
