#include "cloth.h"

Cloth::Cloth(int w, int h)
	: w(w), h(h) {
	// initialize uvPoints with uniform grid
	uvPoints = new GLfloat[2 * w * h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			uvPoints[2 * (i*w + j)]     = j;
			uvPoints[2 * (i*w + j) + 1] = i;
		}
	}

	worldPoints = new GLfloat[3 * w * h];
}

// NOTE: doesn't check bounds!
GLfloat Cloth::worldPoint(int x, int y) {
	return worldPoints[y * w + x];
}
