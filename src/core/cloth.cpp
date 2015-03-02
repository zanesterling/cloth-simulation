#include "cloth.h"

Cloth::Cloth(int w, int h)
	: w(w), h(h) {
	uvPoints = new GLfloat[2 * w * h];
	worldPoints = new GLfloat[3 * w * h];
}

// NOTE: doesn't check bounds!
GLfloat Cloth::worldPoint(int x, int y) {
	return worldPoints[y * w + x];
}
