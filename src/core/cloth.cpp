#include "cloth.h"

Cloth::Cloth(int w, int h)
	: w(w), h(h) {

	initUvPoints();
	initWorldPoints();
}

// initialize uvPoints with uniform grid
void Cloth::initUvPoints() {
	uvPoints = new GLfloat[2 * w * h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			uvPoints[2 * (i*w + j)]     = j;
			uvPoints[2 * (i*w + j) + 1] = i;
		}
	}
}

// initializte world points
void Cloth::initWorldPoints() {
	worldPoints = new GLfloat[3 * w * h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			GLfloat *point = getUvPoint(j, i);
			setWorldPoint(j, i, point[0], point[1], 0);
		}
	}
}

// NOTE: doesn't check bounds!
GLfloat *Cloth::getUvPoint(int x, int y) {
	return uvPoints + 2 * (y*h + x);
}

// NOTE: doesn't check bounds!
GLfloat *Cloth::getWorldPoint(int x, int y) {
	return worldPoints + 3 * (y*w + x);
}

void Cloth::setWorldPoint(int xCor, int yCor, GLfloat xPos, GLfloat yPos,
	GLfloat zPos) {
	GLfloat *point = getWorldPoint(xCor, yCor);
	point[0] = xPos;
	point[1] = yPos;
	point[2] = zPos;
}
